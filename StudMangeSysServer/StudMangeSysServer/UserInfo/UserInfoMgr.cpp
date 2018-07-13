#include "UserInfoMgr.h"
#include "LockTools.h"

UserInfoMgr::UserInfoMgr()
{

}

UserInfoMgr::~UserInfoMgr()
{

}

bool UserInfoMgr::InsertInfo(unsigned __int64 socketId, UserInfo userInfo)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter == m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		m_mUserInfo.insert(pair<unsigned __int64, UserInfo>(socketId, userInfo));
		UserInfoLock::GetInstance()->Unlock();

		return true;
	}
	
	return false;
}

bool UserInfoMgr::InsertInfo(unsigned __int64 socketId, string strIP, unsigned short sPort)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter == m_mUserInfo.end())
	{
		UserInfo userInfo;
		userInfo.strIP = strIP;
		userInfo.sPort = sPort;
		
		UserInfoLock::GetInstance()->Lock();
		m_mUserInfo.insert(pair<unsigned __int64, UserInfo>(socketId, userInfo));
		UserInfoLock::GetInstance()->Unlock();

		printf("%s  socketId[%llu]  strIP[%s]  sPort[%d]\n", __FUNCTION__, socketId, strIP.c_str(), sPort);

		return true;
	}

	return false;
}

bool UserInfoMgr::RemoveInfoBySocketId(unsigned __int64 socketId)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		m_mUserInfo.erase(iter);
		UserInfoLock::GetInstance()->Unlock();

		printf("%s  socketId[%llu]\n", __FUNCTION__, socketId);
	}

	return true;
}

bool UserInfoMgr::RemoveInfoByUserId(unsigned int iUserId)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.begin();
	for (; iter != m_mUserInfo.end(); iter++)
	{
		if (iter->second.iUserId == iUserId)
		{
			UserInfoLock::GetInstance()->Lock();
			m_mUserInfo.erase(iter);
			UserInfoLock::GetInstance()->Unlock();

			printf("%s  iUserId[%u]\n", __FUNCTION__, iUserId);

			break;
		}
	}

	return true;
}

bool UserInfoMgr::SetUserIdBySocketId(unsigned __int64 socketId, unsigned int iUserId)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		m_mUserInfo[socketId].iUserId = iUserId;
		UserInfoLock::GetInstance()->Unlock();

		return true;
	}

	return false;
}

unsigned int UserInfoMgr::GetUserIdBySocketId(unsigned __int64 socketId)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		return m_mUserInfo[socketId].iUserId;
		UserInfoLock::GetInstance()->Unlock();
	}

	return 0; //学号肯定要设置大于0的
}

bool UserInfoMgr::SetAuthorityBySocketId(unsigned __int64 socketId, string strAuthority)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		m_mUserInfo[socketId].strAuthority = strAuthority;
		UserInfoLock::GetInstance()->Unlock();

		return true;
	}

	return false;
}

string UserInfoMgr::GetAuthorityBySocketId(unsigned __int64 socketId)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		return m_mUserInfo[socketId].strAuthority;
		UserInfoLock::GetInstance()->Unlock();
	}

	return ""; 
}

bool UserInfoMgr::SetAccountBySocketId(unsigned __int64 socketId, string strAccount)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		m_mUserInfo[socketId].strAccount = strAccount;
		UserInfoLock::GetInstance()->Unlock();

		return true;
	}

	return false;
}

string UserInfoMgr::GetAccountBySocketId(unsigned __int64 socketId)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		return m_mUserInfo[socketId].strAccount;
		UserInfoLock::GetInstance()->Unlock();
	}

	return ""; 
}

bool UserInfoMgr::SetRegNeedCountBySocketId(unsigned __int64 socketId, short sRegNeedCount)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		m_mUserInfo[socketId].sRegNeedCount = sRegNeedCount;
		UserInfoLock::GetInstance()->Unlock();

		return true;
	}

	return false;
}

short UserInfoMgr::GetRegNeedCountBySocketId(unsigned __int64 socketId)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		return m_mUserInfo[socketId].sRegNeedCount;
		UserInfoLock::GetInstance()->Unlock();
	}

	return -1; 
}