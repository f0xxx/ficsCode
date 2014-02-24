import sys
import re
import string
import os

__version__ = "1.0.0"

def GetDiskInfo():
    diskInfo = os.popen(r'script -q -c "fdisk -l|grep \"Disk /\""').readlines()
    diskInfo1 = os.popen(r'script -q -c "fdisk -l|grep \"contain a valid partition table\""').readlines()
    diskInfo2 = os.popen(r'script -q -c "dmesg|grep \"] Attached\""').readlines()
    diskInfo4 = os.popen(r'script -q -c "pvscan|grep \"PV /dev\""').readlines()

    reRule = 'Disk /dev/(.*): (.*), (.*) bytes'
    reRule1 = 'Attached (\S+) disk'
    reRule2 = 'Status: (\S+)'
    dataType = 'have partition'
    devType = 'unkown'
    devStatus = 'unkown'
    ret = []
    for line in diskInfo:
        if re.search('mapper',line) != None:
            continue
        dataType = 'have partition'
        devType = 'unkown'
        devStatus = 'unkown'
        c = re.search(reRule,line)
        if c != None:
            for line1 in diskInfo1:
                c1 = re.search(c.groups()[0],line1)
                if c1 != None:
                    dataType = 'no used'
                    for line4 in diskInfo4:
                        reRule4 = '/dev/' + c.groups()[0] + '.*fics(\S+)'
                        c4 = re.search(reRule4,line4)
                        if c4 != None:
                            dataType = c4.groups()[0]
                            break
                    break
            for line2 in diskInfo2:
                c2 = re.search(c.groups()[0],line2)
                if c2 != None:
                    cc2 = re.search(reRule1,line2)
                    if cc2 != None:
                        devType = cc2.groups()[0]
                    break
            cmdStr = 'script -q -c \"smartctl -H /dev/' + c.groups()[0] + '\"'
            diskInfo3 = os.popen(cmdStr).readlines()
            if diskInfo3 != None:
                c3 = re.search(reRule2,diskInfo3[-1])
                if c3 != None:
                    devStatus = c3.groups()[0]
            tp = [c.groups()[0],c.groups()[1],dataType,devType,devStatus,long(c.groups()[2])]
            ret.append(tp)

    print ret
    return ret

def MakePool(poolType,cmdStr):
    cmd = r'script -q -c "pvcreate '+ cmdStr + r'"|grep -v "leaked on"'
    ret = os.popen(cmd).readlines()
    reRule = '(successfully created)'
    for line in ret:
        c = re.search(reRule,line)
        if c == None:
            print cmd,ret
            return 1,cmd,line[0:-2]

    if poolType == 'meta':
        cmd = r'script -q -c "vgcreate /dev/ficsMeta '+ cmdStr + r'"|grep -v "leaked on"'
    else:
        cmd = r'script -q -c "vgcreate /dev/ficsData '+ cmdStr + r'"|grep -v "leaked on"'
    ret = os.popen(cmd).readlines()
    for line in ret:
        c = re.search(reRule,line)
        if c == None:
            print cmd,ret
            return 1,cmd,line[0:-2]

    pvNumStr = ''
    stripSizeParStr = ' -I 1024 '
    peSizeStr = ''
    if poolType == 'meta':
        cmd = r'script -q -c "vgdisplay ficsMeta"|grep -v "leaked on"'
    else:
        cmd = r'script -q -c "vgdisplay ficsData"|grep -v "leaked on"'
    reRule = '(not found)'
    reRule1 = 'Cur PV\s+(\d+)'
    reRule2 = 'Total PE\s+(\d+)'
    ret = os.popen(cmd).readlines()
    for line in ret:
        c = re.search(reRule,line)
        if c != None:
            print cmd,ret
            return 1,cmd,line[0:-2]
        c = re.search(reRule1,line)
        if c != None:
            pvNumStr = '-i '+ c.groups()[0]
        c = re.search(reRule2,line)
        if c != None:
            peSizeStr = '-l '+ c.groups()[0]

    if poolType == 'meta':
        cmd = r'script -q -c "lvcreate ' + pvNumStr + stripSizeParStr + peSizeStr + r' -n meta ficsMeta"|grep -v "leaked on"'
    else:
        cmd = r'script -q -c "lvcreate ' + pvNumStr + stripSizeParStr + peSizeStr + r' -n data ficsData"|grep -v "leaked on"'
    ret = os.popen(cmd).readlines()
    for line in ret:
        c = re.search("(created)",line)
        if c == None:
            print cmd,ret
            return 1,cmd,line[0:-2]
    print cmd,ret
    return 0,cmd,line[0:-2]

def RemovePool():
    isHaveMetaPool = 0
    isHaveDataPool = 0
    ret = os.popen(r'script -q -c "lvscan"|grep -v "leaked on"').readlines()
    reRule = '(/dev/ficsMeta/meta)'
    reRule1 = '(/dev/ficsData/data)'
    for line in ret:
        c = re.search(reRule,line)
        if c != None:
            isHaveMetaPool = 1
        c = re.search(reRule1,line)
        if c != None:
            isHaveDataPool = 1

    if isHaveMetaPool == 1:
        ret = os.popen(r'script -q -c "lvremove -f /dev/ficsMeta/meta"|grep -v "leaked on"').readlines()
        ret += os.popen(r'script -q -c "vgremove -f ficsMeta"|grep -v "leaked on"').readlines()
    if isHaveDataPool == 1:
        ret += os.popen(r'script -q -c "lvremove -f /dev/ficsData/data"|grep -v "leaked on"').readlines()
        ret += os.popen(r'script -q -c "vgremove -f ficsData"|grep -v "leaked on"').readlines()
    reRule = '(successfully removed)'
    for line in ret:
        c = re.search(reRule,line)
        if c == None:
            print ret
            return 1,line[0:-2]

    ret = os.popen(r'script -q -c "pvscan"|grep -v "leaked on"').readlines()
    reRule = 'PV (/dev/\S+)'
    cmdStr = ''
    for line in ret:
        c = re.search(reRule,line)
        if c != None:
            cmdStr += c.groups()[0] + ' '
   
    ret = os.popen(r'script -q -c "pvremove -f '+cmdStr+r'"|grep -v "leaked on"').readlines()
    reRule = '(successfully wiped)'
    for line in ret:
        c = re.search(reRule,line)
        if c == None:
            print ret
            return 1,line[0:-2]

    print ret
    return 0,line[0:-2]

def GetPoolUsed():
    retData = []
    ret = os.popen(r'script -q -c "df|grep fics"').readlines()
    reRule = 'fics(\S+)-.*\s+(\d+)\s+(\d+)\s+(\d+)%\s+(\S+)'
    for line in ret:
        c = re.search(reRule,line)
        if c != None:
            tp = [c.groups()[0],long(c.groups()[1]),long(c.groups()[2]),long(c.groups()[3]),c.groups()[4]]
            retData.append(tp)
            
    print retData
    return retData

##############################################################################################
#GetDiskInfo()
#MakePool("meta","/dev/sdd /dev/sde")
#RemovePool()
#GetPoolUsed()

