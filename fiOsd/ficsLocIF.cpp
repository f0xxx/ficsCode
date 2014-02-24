#include"ficsLocIF.h"
#include "../fiOsd/fiOsd.h"
#undef OSD_DEBUG_NONET			//close loc readwrite rand test

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  fileOp  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

fileOp::fileOp(string& filePath) : fp(NULL)
{
	fp = fopen(filePath.c_str(), "rb+");
	if (NULL == fp)
	{
//  	LOCIF_LOG_ERR("fopen[%s] rb+ failed err[%s]",filePath.c_str(),strerror(errno));
	FILEOP_REOPEN:
		fp = fopen(filePath.c_str(), "wb+");
		if (NULL == fp)
		{
//  		LOCIF_LOG_ERR("fopen[%s] wb+ failed err[%s]",filePath.c_str(),strerror(errno));
			if (2 == errno)
			{
				s32 pos = filePath.find('/',1);
				while (pos != string::npos)
				{
					if((access(filePath.substr(0, pos).c_str(),F_OK)) == -1)  
					{
						LOCIF_LOG_DBG("%s is not exist",filePath.substr(0, pos).c_str());
						if (-1 == mkdir(filePath.substr(0, pos).c_str(), 0777))
						{
							LOCIF_LOG_ERR("mkdir[%s] failed err[%s]", filePath.substr(0, pos).c_str(), strerror(errno));
							return;
						}
					}
					pos = filePath.find('/',pos+1);
				}
				goto FILEOP_REOPEN;
			}
			return;
		}
	}
}

fileOp::~fileOp()
{
	if (NULL != fp)
	{
		fclose(fp);
		fp = NULL;
	}
}

FiErr fileOp::ReadData(char *buf, unsigned int& len, unsigned long offset)
{
	FiErr ret;

#ifdef OSD_DEBUG_NONET
	if (rand() % 88 == 0)
	{
		LOCIF_LOG_ERR("fread failed err[dbg rand failed]---len[0x%x]", len);
		ret = FErr(MDisk_freadFailed);
		return ret;
	}
#endif

	fpLock.Lock();

	int retTmp = fseek(fp, offset, SEEK_SET);
	if (0 != retTmp)
	{
		LOCIF_LOG_ERR("fseek failed err[%s]---offset[0x%lx]", strerror(errno), offset);
		ret = FErrEx(MDisk_fseekFailed, errno);
		len = 0;
		goto ReadDataEnd;
	}

	retTmp = fread(buf, 1, len, fp);
	if (len != retTmp)
	{
		LOCIF_LOG_ERR("fread failed,err[%s]---len[0x%x],retTmp[0x%x]", strerror(errno), len, retTmp);
		len = retTmp;
		if (errno == ENOENT)
		{
			goto ReadDataEnd;
		}

		if (!feof(fp))
		{
			RELEASE_ASSERT_FASLE;
			ret = FErrEx(MDisk_freadFailed, errno);
			goto ReadDataEnd;
		}
	}

ReadDataEnd:
	fpLock.Unlock();

	return ret;
}

FiErr fileOp::WriteData(char *buf, unsigned int len, unsigned long offset, bool isFlush)
{
	FiErr ret;

#ifdef OSD_DEBUG_NONET
	if (rand() % 88 == 0)
	{
		LOCIF_LOG_ERR("fwrite failed err[dbg rand failed]---len[0x%x]", len);
		ret = FErr(MDisk_fwriteFailed);
		return ret;
	}
#endif

	fpLock.Lock();

	int retTmp = fseek(fp, offset, SEEK_SET);
	if (0 != retTmp)
	{
		LOCIF_LOG_ERR("fseek failed err[%s]---offset[0x%lx]", strerror(errno), offset);
		ret = FErrEx(MDisk_fseekFailed, errno);
		goto WriteDataEnd;
	}

	retTmp = fwrite(buf, 1, len, fp);
	if (len != retTmp)
	{
		LOCIF_LOG_ERR("fwrite failed err[%s]---len[0x%x],retTmp[0x%x]", strerror(errno), len, retTmp);
		RELEASE_ASSERT_FASLE;
		ret = FErrEx(MDisk_fwriteFailed, errno);
		goto WriteDataEnd;
	}

	if (isFlush)
	{
		fflush(fp);
		fsync(fileno(fp));
	}

WriteDataEnd:
	fpLock.Unlock();

	return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  fileIF /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

fileIF::fileIF(string& filePathT, unsigned int fileOpNumT) : filePath(filePathT), fp(NULL), fileOPNum(fileOpNumT)
{
	if (1 == fileOpNumT)
	{
		fp = new fileOp(filePath);
		if (NULL == fp)
		{
			LOCIF_LOG_ERR("new failed err[NULL == fp]");
		}
		return;
	}

	int i = 0;
	for (; i < fileOpNumT; ++i)
	{
		fp = new fileOp(filePath);
		if (NULL == fp)
		{
			LOCIF_LOG_ERR("new failed err[NULL == fp]");
			break;
		}
		fpAry.push_back(fp);
	}
	if (i < fileOpNumT)
	{
		for (--i; i >= 0; --i)
		{
			RELEASE_ASSERT(fpAry[i] != NULL);
			delete fpAry[i];
		}
		fpAry.clear();
		return;
	}
	fpAryIt = fpAry.begin();
}

fileIF::~fileIF()
{
	if (1 == fileOPNum)
	{
		if (NULL != fp)
		{
			delete fp;
		}
		return;
	}

	for (fpAryIt = fpAry.begin(); fpAryIt != fpAry.end(); ++fpAryIt)
	{
		RELEASE_ASSERT(*fpAryIt != NULL);
		delete *fpAryIt;
	}
	fpAry.clear();
}

FiErr fileIF::ReadData(char *buf, unsigned int& len, unsigned long offset)
{
	RELEASE_ASSERT(buf != NULL);
	if (0 == len)
	{
		return FSuc(0);
	}
	return GetOneFileOp()->ReadData(buf, len, offset);
}

FiErr fileIF::WriteData(char *buf, unsigned int len, unsigned long offset, bool isFlush)
{
	FiErr ret;

	RELEASE_ASSERT(buf != NULL);
	if (0 == len)
	{
		return FSuc(0);
	}
	if (fileOPNum > 1)
	{
		ret = GetOneFileOp()->WriteData(buf, len, offset, true);
	} else
	{
		ret = GetOneFileOp()->WriteData(buf, len, offset, isFlush);
	}

	return ret;
}

inline string& fileIF::GetFilePath() { return filePath;}
inline unsigned int fileIF::GetFileOPSize() { return fileOPNum;}

fileOp* fileIF::GetOneFileOp()
{
	if (fileOPNum == 1)
	{
		return fp;
	}

	fpAryLock.Lock();
	fp = *fpAryIt;
	if (++fpAryIt == fpAry.end())
	{
		fpAryIt = fpAry.begin();
	}
	fpAryLock.Unlock();
	return fp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  ficsLocIF ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
fileIF* ficsLocIF::GetFileIF(const char *filePath, unsigned int fileOpNumT)
{
	string strT(filePath);
	return GetFileIF(strT, fileOpNumT);
}

fileIF* ficsLocIF::GetFileIF(string& filePath, unsigned int fileOpNumT)
{
	fileIF *fif = NULL;

	fileIFLock.Lock();
	if (fileIFMap.empty())
	{
		fif = AddOneFileIF(filePath, fileOpNumT);
		goto GetFileIFEnd;
	}

	fileIFMapIt = fileIFMap.find(filePath);
	if (fileIFMap.end() != fileIFMapIt)
	{
		fif = fileIFMapIt->second;
		fileIFAryIt = find(fileIFAry.begin(), fileIFAry.end(), fif);
		RELEASE_ASSERT(fileIFAry.end() != fileIFAryIt);
		fileIFAry.erase(fileIFAryIt);

		if (fif->GetFileOPSize() < fileOpNumT)
		{
			fileIFMap.erase(fif->GetFilePath());
			delete fif;
			fif = AddOneFileIF(filePath, fileOpNumT);
		} else
		{
			fileIFAry.push_back(fif);
		}
	} else
	{
		fif = AddOneFileIF(filePath, fileOpNumT);
	}

GetFileIFEnd:
	fileIFLock.Unlock();

	return fif;
}

inline fileIF* ficsLocIF::AddOneFileIF(string& filePath, unsigned int fileOpNumT)
{
	fileIF *fif = new fileIF(filePath, fileOpNumT);
	if (fileIFAry.size() == FILEIFCACHESIZE)
	{
		fileIF *fifDel = fileIFAry[0];
		fileIFMap.erase(fifDel->GetFilePath());
		fileIFAry.erase(fileIFAry.begin());
		delete fifDel;
	}
	fileIFMap.insert(map<string, fileIF *>::value_type(filePath, fif));
	fileIFAry.push_back(fif);
	return fif;
}

inline void ficsLocIF::DelOneFileIF(string& filePath)
{
	fileIF *fif = NULL;
	fileIFLock.Lock();
	if (fileIFMap.empty())
	{
		goto GetFileIFEnd;
	}

	fileIFMapIt = fileIFMap.find(filePath);
	if (fileIFMap.end() != fileIFMapIt)
	{
		fif = fileIFMapIt->second;
		fileIFAryIt = find(fileIFAry.begin(), fileIFAry.end(), fif);
		RELEASE_ASSERT(fileIFAry.end() != fileIFAryIt);
		fileIFAry.erase(fileIFAryIt);
		fileIFMap.erase(fif->GetFilePath());
		delete fif;
	}

GetFileIFEnd:
	fileIFLock.Unlock();
}

ficsLocIF::ficsLocIF()
{
	fileIFAry.reserve(FILEIFCACHESIZE);
}

ficsLocIF::~ficsLocIF()
{
	for_each(fileIFAry.begin(), fileIFAry.end(), DelFileIF());
	fileIFAry.clear();
	fileIFMap.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Init FicsLocIF  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void InitFicsLocIF()
{
#ifdef __WIN32__
	ficsLocIF::GetFicsLocIF();  //mutithread safe on gcc 4.6.3 but not support on vs2008
#endif
}
