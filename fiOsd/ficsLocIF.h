///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  hunter code ([hunter.f0x] remeber my brothers) email:hunterlhy@163.com /////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __FICSLOCIF_H__
#define __FICSLOCIF_H__
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "../public/osdTools.h"
using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOCIF_LOG_ERR OSD_LOG_ERR	//VTRACE
#define LOCIF_LOG_DBG OSD_LOG_DBG	//STRACE

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define FILEIFCACHESIZE 100

struct NodeAndTruncateNo
{
	s32 nodeID;
	u32 truncateNo;
};

struct fileOp
{
	friend struct fileIF;

	FiErr ReadData(char *buf, unsigned int &len, unsigned long offset);
	FiErr WriteData(char *buf,unsigned int len,unsigned long offset,bool isFlush);

private:
	fileOp(string &filePath);
	~fileOp();

private:
	FILE *fp;
	OsdLock fpLock;
};

struct fileIF
{
	typedef vector<fileOp*>::iterator FPARY_ITR;

	friend struct ficsLocIF;

	/**
	 * \读取数据，此接口为原子操作
	 *  
	 * @param buf 读取数据buf
	 * @param len 读取长度
	 * @param offset 偏移值
	 *  
	 * @return true表示成功，false表示失败  
	 */
	FiErr ReadData(char *buf, unsigned int &len, unsigned long offset);		//Atom Op
	/**
	 * \写入数据，此接口为原子操作
	 *  
	 * @param buf 读取数据buf
	 * @param len 读取长度
	 * @param offset 偏移值
	 *  
	 * @return true表示成功，false表示失败  
	 */
	FiErr WriteData(char *buf, unsigned int len, unsigned long offset,bool isFlush = true);		//Atom Op

	/**
	 * \获取文件路径
	 */
	string& GetFilePath();
	/**
	 * \获取句柄个数
	 */
	unsigned int GetFileOPSize();

	/**
	 * \获取一个fileOp对象 useless
	 */
	fileOp* GetOneFileOp();

private:
	fileIF(string &filePathT,unsigned int fileOpNumT);
	~fileIF();

private:
	string filePath;

	fileOp *fp;

	vector<fileOp*> fpAry;
	FPARY_ITR fpAryIt;
	OsdLock fpAryLock;
	unsigned int fileOPNum;
};

struct ficsLocIF
{
	typedef map<string,fileIF*>::iterator FILEIFMAP_ITR;
	typedef vector<fileIF*>::iterator FILEIFARY_ITR;

	/**
	 * \获取ficsLocIF单件实例 
	 */
	static ficsLocIF& GetFicsLocIF() {static ficsLocIF flif; return flif;}

	/**
	 * \获取fileIF对象
	 *  
	 * @param filePath 文件路径
	 * @param fileOpNumT 句柄类个数 
	 *  				 预防类似windows下的句柄锁导致的性能问题
	 *  
	 * @return 不为NULL表示成功，NULL表示失败  
	 */
	fileIF* GetFileIF(const char *filePath,unsigned int fileOpNumT = 1);
	fileIF* GetFileIF(string &filePath,unsigned int fileOpNumT = 1);
	inline void DelOneFileIF(string& filePath);

	inline u32 FileSize(string &filePath)
	{
	    struct stat buf;
		if(stat(filePath.c_str(),&buf) == -1)
		{
			return 0;
		}
		return (unsigned long)buf.st_size;
	}

//	inline void ChangeToPoint(string &filePath)
//	{
//		string newFilePath = string(filePath)+string("_pointer");
//		if((rename(filePath.c_str(),newFilePath.c_str())) == -1)
//		{
//			RELEASE_ASSERT_FASLE;
//		}
//	}
	
	inline bool IsExist(string &filePath)
	{
		if((access(filePath.c_str(),F_OK)) != -1)  
		{
			return true;
		}
		LOCIF_LOG_DBG("%s is not exist",filePath.c_str());
		return false;
	}

	bool RemoveFile(const string &delPath)
	{
		bool ret = true;
		if (remove(delPath.c_str()) == -1)
		{
			LOCIF_LOG_ERR("remove[%s] failed err[%s]",delPath.c_str(),strerror(errno));
			if (errno != ENOENT)
			{
				ret = false;
			}
		}
		return ret;
	}

	FiErr GetTruncateRecoverAry(string path,vector<NodeAndTruncateNo> &ntAry,u32 truncateNoBegin,u32 truncateNoEnd)
	{
		FiErr ret;
		if (path.empty())
		{
			return ret;
		}

		dirLock.Lock();
		char oldPath[256] = {0};
		getcwd(oldPath,256);
		
		if(chdir(path.c_str()) == -1)
		{
			LOCIF_LOG_ERR("chdir[%s] failed err[%s]",path.c_str(),strerror(errno));
			if (errno == ENOENT)
			{
				dirLock.Unlock();
				return ret;
			}
			else
			{
				ret = FErrEx(MDisk_TruncateDirFailed, errno);
				dirLock.Unlock();
				return ret;
			}
		}

		ret = DoGetTruncateRecoverAry(ntAry,truncateNoBegin,truncateNoEnd,-1);
		RELEASE_ASSERT(chdir(oldPath) == 0);
		dirLock.Unlock();
	}

	FiErr TruncateDir(string &truncatePath, u32 truncateNo, u32 truncateSize)
	{
		FiErr ret;
		if (truncatePath.empty())
		{
			return ret;
		}
	 
		dirLock.Lock();
		char oldPath[256] = {0};
		getcwd(oldPath,256);
		
		if(chdir(truncatePath.c_str()) == -1)
		{
			LOCIF_LOG_ERR("chdir[%s] failed err[%s]",truncatePath.c_str(),strerror(errno));
			if (errno == ENOENT)
			{
				dirLock.Unlock();
				return ret;
			}
			else
			{
				ret = FErrEx(MDisk_TruncateDirFailed, errno);
				dirLock.Unlock();
				return ret;
			}
		}

		ret = DoTruncateDir(truncateNo,truncateSize);
		RELEASE_ASSERT(chdir(oldPath) == 0);
		dirLock.Unlock();
		return ret;
	}

	bool RemoveDir(string &delPath,bool isDelPath = true)
	{
		bool ret = true;
		if (delPath.empty())
		{
			return ret;
		}

		dirLock.Lock();
		char oldPath[256] = {0};
		getcwd(oldPath,256);
		
		if(chdir(delPath.c_str()) == -1)
		{
			LOCIF_LOG_ERR("chdir[%s] failed err[%s]",delPath.c_str(),strerror(errno));
			if (errno == ENOENT)
			{
				dirLock.Unlock();
				return ret;
			}
			else
			{
				dirLock.Unlock();
				return false;
			}
		}
		DoRemoveDir();
		RELEASE_ASSERT(chdir(oldPath) == 0);

		if (isDelPath)
		{
			if (remove(delPath.c_str()) == -1)
			{
				LOCIF_LOG_ERR("remove[%s] failed err[%s]",delPath.c_str(),strerror(errno));
				if (errno != ENOENT)
				{
					dirLock.Unlock();
					ret = false;
				}
			}
		}

//		LOCIF_LOG_ERR("===RemoveFile=== done delPath[%s],ret[%s]",delPath.c_str(),ret?"true":"false");
		dirLock.Unlock();
		return ret;
	}

	void EnumDir(string &enumPath,vector<string> &retPaths)
	{
		if (enumPath.empty())
		{
			return;
		}

		if('/' != enumPath[enumPath.length()-1])
		{
			enumPath += "/";
		}

		dirLock.Lock();
		char oldPath[256] = {0};
		getcwd(oldPath,256);
		
		if(chdir(enumPath.c_str()) == -1)
		{
			LOCIF_LOG_ERR("chdir[%s] failed err[%s]",enumPath.c_str(),strerror(errno));
			if (errno == ENOENT)
			{
				dirLock.Unlock();
				return;
			}
			else
			{
				dirLock.Unlock();
				return;
			}
		}
		DoEnumDir(retPaths);
		RELEASE_ASSERT(chdir(oldPath) == 0);
		dirLock.Unlock();
	}

	void LogFullRecover(string &enumPath,s32 node)
	{
		if (enumPath.empty())
		{
			return;
		}

		if('/' != enumPath[enumPath.length()-1])
		{
			enumPath += "/";
		}

		dirLock.Lock();
		char oldPath[256] = {0};
		getcwd(oldPath,256);
		
		if(chdir(enumPath.c_str()) == -1)
		{
			LOCIF_LOG_ERR("chdir[%s] failed err[%s]",enumPath.c_str(),strerror(errno));
			if (errno == ENOENT)
			{
				dirLock.Unlock();
				return;
			}
			else
			{
				dirLock.Unlock();
				return;
			}
		}
		DoLogFullRecover(enumPath,node);
		RELEASE_ASSERT(chdir(oldPath) == 0);
		dirLock.Unlock();
	}
	
	bool TruncateFile(string truncatePath,s64 truncateSize)
	{
		bool ret = true;
		if(truncate(truncatePath.c_str(),truncateSize) == -1)
		{
			ret = false;
			LOCIF_LOG_ERR("truncate[%s] failed err[%s]",truncatePath.c_str(),strerror(errno));
		}
		return ret;
	}

private:

	void DoLogFullRecover(string &enumPath,s32 node)
	{
		struct dirent *ent = NULL;
		struct stat st;
		string testPath;
		char idStr[25] = {0};
	 
		DIR *curDir = opendir(".");
		if (curDir == NULL)
		{
			LOCIF_LOG_ERR("opendir[.] failed err[%s]",strerror(errno));
			return;
		}
	 
		while ((ent = readdir(curDir)) != NULL)
		{
			RELEASE_ASSERT(stat(ent->d_name, &st) == 0);
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
			{
				continue;
			}

			sprintf(idStr,"/rddcNode_%d",node);
			testPath = enumPath + ent->d_name + idStr;
			if (IsExist(testPath))
			{
				;//???
			}
		}
		
		closedir(curDir);
	}

	void DoEnumDir(vector<string> &retPaths)
	{
		struct dirent *ent = NULL;
		struct stat st;
	 
		DIR *curDir = opendir(".");
		if (curDir == NULL)
		{
			LOCIF_LOG_ERR("opendir[.] failed err[%s]",strerror(errno));
			return;
		}
	 
		while ((ent = readdir(curDir)) != NULL)
		{
			RELEASE_ASSERT(stat(ent->d_name, &st) == 0);
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
			{
				continue;
			}
	  
			retPaths.push_back(ent->d_name);
		}
		
		closedir(curDir);
	}


	FiErr DoGetTruncateRecoverAry(vector<NodeAndTruncateNo> &ntAry,u32 truncateNoBegin,u32 truncateNoEnd,s32 nid)
	{
		FiErr ret;
		struct dirent *ent = NULL;
		struct stat st;
	 
		DIR *curDir = opendir(".");
		if (curDir == NULL)
		{
			LOCIF_LOG_ERR("opendir[.] failed err[%s]",strerror(errno));
			ret = FErrEx(MDisk_TruncateDirFailed, errno);
			return ret;
		}
	 
		while ((ent = readdir(curDir)) != NULL)
		{
			RELEASE_ASSERT(stat(ent->d_name, &st) == 0);
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
			{
				continue;
			}
	 
			if (S_ISDIR(st.st_mode))
			{
				RELEASE_ASSERT(chdir(ent->d_name) == 0);
				DoGetTruncateRecoverAry(ntAry,truncateNoBegin,truncateNoEnd,str2u32(ent->d_name));
				RELEASE_ASSERT(chdir("..") == 0);
				continue;
			}

			if (-1 == nid)
			{
				continue; 
			}
	 
			u32 index = 0;
			while (ent->d_name[strlen(ent->d_name) - (++index)] != 'a'){};
			u32 currentNo = str2u32(&ent->d_name[strlen(ent->d_name) - index + 1]);

			NodeAndTruncateNo nt;
			if (currentNo >= truncateNoBegin && currentNo <= truncateNoEnd)
			{
				nt.nodeID = nid;
				nt.truncateNo = currentNo;
				ntAry.push_back(nt);
			}
		}
		
		closedir(curDir);
		return ret;
	}

	FiErr DoTruncateDir(u32 truncateNo,u32 truncateSize)
	{
		FiErr ret;
		struct dirent *ent = NULL;
		struct stat st;
	 
		DIR *curDir = opendir(".");
		if (curDir == NULL)
		{
			LOCIF_LOG_ERR("opendir[.] failed err[%s]",strerror(errno));
			ret = FErrEx(MDisk_TruncateDirFailed, errno);
			return ret;
		}
	 
		while ((ent = readdir(curDir)) != NULL)
		{
			RELEASE_ASSERT(stat(ent->d_name, &st) == 0);
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
			{
				continue;
			}
	 
			if (S_ISDIR(st.st_mode))
			{
				RELEASE_ASSERT(chdir(ent->d_name) == 0);
				DoTruncateDir(truncateNo,truncateSize);
				RELEASE_ASSERT(chdir("..") == 0);
				continue;
			}
	 
			u32 index = 0;
			while (ent->d_name[strlen(ent->d_name) - (++index)] != 'a'){};
			u32 currentNo = str2u32(&ent->d_name[strlen(ent->d_name) - index + 1]);

			if (currentNo > truncateNo)
			{
				if(remove(ent->d_name) == -1)
				{
					LOCIF_LOG_ERR("remove[%s] failed err[%s]",ent->d_name,strerror(errno));
					ret = FErrEx(MDisk_TruncateDirFailed, errno);
					break;
				}
			}
			else if (truncateSize != 0 && currentNo == truncateNo)
			{
				if(truncate(ent->d_name,truncateSize) == -1)
				{
					LOCIF_LOG_ERR("truncate[%s] failed err[%s]",ent->d_name,strerror(errno));
					ret = FErrEx(MDisk_TruncateDirFailed, errno);
					break;
				}
			}
		}
		
		closedir(curDir);
		return ret;
	}

	void DoRemoveDir()
	{
		struct dirent *ent = NULL;
		struct stat st;
	 
		DIR *curDir = opendir(".");
		if (curDir == NULL)
		{
			LOCIF_LOG_ERR("opendir[.] failed err[%s]",strerror(errno));
			return;
		}
	 
		while ((ent = readdir(curDir)) != NULL)
		{
			RELEASE_ASSERT(stat(ent->d_name, &st) == 0);
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
			{
				continue;
			}
	 
			if (S_ISDIR(st.st_mode))
			{
				RELEASE_ASSERT(chdir(ent->d_name) == 0);
				DoRemoveDir();
				RELEASE_ASSERT(chdir("..") == 0);
			}
	 
			if(remove(ent->d_name) == -1)
			{
				LOCIF_LOG_ERR("remove[%s] failed err[%s]",ent->d_name,strerror(errno));
				break;
			}
		}
		
		closedir(curDir);
	}

	struct DelFileIF 
	{
		inline void operator()(fileIF *fif)
		{
			delete fif;
		}
	};

	inline fileIF* AddOneFileIF(string &filePath,unsigned int fileOpNumT);
	ficsLocIF();
	~ficsLocIF();

private:
	map<string,fileIF*> fileIFMap;
	FILEIFMAP_ITR fileIFMapIt;
	vector<fileIF*> fileIFAry;
	FILEIFARY_ITR fileIFAryIt;
	OsdLock fileIFLock;
	OsdLock dirLock;
};

void InitFicsLocIF();

#endif
