#include "AddScoreSysProc.h"
#include "ProcMgr.h"
#include "TCPHandle.h"
#include "CheckTool.h"
#include "UserInfoMgr.h"
#include "StringTool.h"

AddScoreSysProc::AddScoreSysProc(ProcDef nProcDef) : BaseProc(nProcDef)
{
	initMapChoose();
}

AddScoreSysProc::~AddScoreSysProc()
{

}


bool AddScoreSysProc::initMapChoose()
{
	m_mChoose.insert(pair<int, ChooseData>(GetMaxRealChoose(), ChooseData("单科批量增加成绩", OPER_PER_ADDBATCHSCOREBYONESUBJECT, PROC_DEF_ADDSCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("现有所有科目批量增加成绩", OPER_PER_ADDBATCHSCOREBYSUBJECTS, PROC_DEF_ADDSCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("单科单条增加成绩", OPER_PER_ADDSINGLESCOREBYONESUBJECT, PROC_DEF_ADDSCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("现有所有科目单条增加成绩", OPER_PER_ADDSINGLESCOREBYSUBJECTS, PROC_DEF_ADDSCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("返回", OPER_PER_INVALID, PROC_DEF_SCORESYSPROC)));

	return true;
}

void AddScoreSysProc::EndProc()
{
	__super::EndProc();
}

void AddScoreSysProc::StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId)
{
	bool bRes = false;
	switch(iAssistId)
	{
	case ASSIST_ID_GET_SUBJECTS_ACK:
		bRes = GetSubjectsAfterAddSingleScoreRecvHandle(vpData, DataLen);
		break;
	case ASSIST_ID_ADD_SINGLE_SCORE_ACK:
		bRes = AddSingleScoreRecvHandle(vpData, DataLen);
		break;
	default:
		printf("%s iAssistId[%d] error\n", __FUNCTION__, iAssistId);
		//ProcMgr::GetInstance()->ProcSwitch(GetMyProcDef(), true); //重新登录注册
		SetIEndFlag(-1);
		break;
	}


	if (GetIEndFlag() == -1)
	{
		ProcMgr::GetInstance()->ProcSwitch(GetMyProcDef(), true);
	}
	else if (GetIEndFlag() == 1)
	{
		ProcMgr::GetInstance()->ProcSwitch(GetNextProc()); //处理成功，切换到下一个界面
	}
	EndRecv(); //有使用GetNextProc()就要先切换再清空数据  但是可能这样做会有问题，如果切换回来同一个界面
}

void AddScoreSysProc::EndRecv()
{
	__super::EndRecv();
}

void AddScoreSysProc::SwitchToOper(OperPermission CurOper)
{
	__super::SwitchToOper(CurOper);


	switch(CurOper)
	{
	case OPER_PER_ADDBATCHSCOREBYONESUBJECT: //单科批量增加成绩
		break;
	case OPER_PER_ADDBATCHSCOREBYSUBJECTS: //现有所有科目批量增加成绩
		break;
	case OPER_PER_ADDSINGLESCOREBYONESUBJECT: //单科单条增加成绩  操作前请求处理
	case OPER_PER_ADDSINGLESCOREBYSUBJECTS: //现有所有科目单条增加成绩  操作前请求处理
		GetSubjectsAfterAddSingleScoreChooseHandle();
		break;
	default:
		printf("%s 没有该操作！\n", __FUNCTION__);
		ProcMgr::GetInstance()->ProcSwitch(GetMyProcDef(), true);  
		break;
	}
}

void AddScoreSysProc::GetSubjectsAfterAddSingleScoreChooseHandle()
{
	//获取现有科目
	CS_MSG_GET_SUBJECTS_REQ SendReq;
	SendReq.sGetType = 2;

	TCPHandle::GetInstance()->Send(&SendReq, sizeof(SendReq), /*MAIN_ID_LOGINREGISTER,*/ ASSIST_ID_GET_SUBJECTS_REQ);
}

//单科单条增加成绩
void AddScoreSysProc::AddSingleScoreByOneSubjectChooseHandle(char* pStrExistSubjects)
{
	if (NULL == pStrExistSubjects)
	{
		printf("%s 科目字段为NULL\n", __FUNCTION__);
		SetIEndFlag(-1);
		return;
	}

	vector<string> vecStrTmp = StringTool::Splite(pStrExistSubjects, "|");
	map<int, string> mStrShowTmp; //显示的科目
	for (unsigned i=0; i<vecStrTmp.size(); i++)
	{
		int sId = (int)atoi(vecStrTmp.at(i).c_str());
		if (UserInfoMgr::GetInstance()->CanFindSubjectsType((SubjectsType)sId))
		{
			mStrShowTmp.insert(pair<int, string>(sId, UserInfoMgr::GetInstance()->GetChineseNameByType((SubjectsType)sId)));
		}
	}

	if (mStrShowTmp.empty())
	{
		printf("没有科目，请先添加科目\n");
		SetIEndFlag(1);
		return;
	}

	cout<<"请添加分数对象的用户名："<<endl;
	string strAccount;
	cin>>strAccount;
	if (!(CheckTool::CheckStringLen(strAccount, 30) && CheckTool::CheckStringByValid(strAccount, "A~Z|a~z|0~9")))
	{
		OperInputErrorHandle(false);
		return;
	}

	printf("请选择要添加分数的科目ID：\n");
	ShowSubjects(mStrShowTmp);
	string iChooseTmp;
	cin>>iChooseTmp;
	if (!CheckTool::CheckStringByValid(iChooseTmp, "0~9") || mStrShowTmp.find((int)atoi(iChooseTmp.c_str())) == mStrShowTmp.end())
	{
		OperInputErrorHandle(false);
		return;
	}

	printf("请输入该科目分数：\n");
	string strScore;
	cin>>strScore;
	if (!(CheckTool::CheckStringLen(strScore, 3) && CheckTool::CheckStringByValid(strScore, "0~9") && (int)atoi(strScore.c_str()) >= 0 && (int)atoi(strScore.c_str()) <= 100))
	{
		OperInputErrorHandle(false);
		return;
	}

	CS_MSG_ADD_SINGLE_SCORE_REQ SendReq;
	SendReq.sType = 1;
	strcpy_s(SendReq.cAccount, sizeof(SendReq.cAccount), strAccount.c_str());
	strcpy_s(SendReq.sSubjectId, sizeof(SendReq.sSubjectId), iChooseTmp.c_str());
	strcpy_s(SendReq.sScore, sizeof(SendReq.sScore), strScore.c_str());

	TCPHandle::GetInstance()->Send(&SendReq, sizeof(SendReq), /*MAIN_ID_LOGINREGISTER,*/ ASSIST_ID_ADD_SINGLE_SCORE_REQ);
}

//现有所有科目单条增加成绩
void AddScoreSysProc::AddSingleScoreBySubjectsChooseHandle(char* pStrExistSubjects)
{
	if (NULL == pStrExistSubjects)
	{
		printf("%s 科目字段为NULL\n", __FUNCTION__);
		SetIEndFlag(-1);
		return;
	}

	vector<string> vecStrTmp = StringTool::Splite(pStrExistSubjects, "|");
	vector<int> vecSubjectId;
	map<int, string> mStrShowTmp; //显示的科目
	for (unsigned i=0; i<vecStrTmp.size(); i++)
	{
		int sId = (int)atoi(vecStrTmp.at(i).c_str());
		if (UserInfoMgr::GetInstance()->CanFindSubjectsType((SubjectsType)sId))
		{
			mStrShowTmp.insert(pair<int, string>(sId, UserInfoMgr::GetInstance()->GetChineseNameByType((SubjectsType)sId)));
			vecSubjectId.push_back(sId);
		}
	}

	if (mStrShowTmp.empty())
	{
		printf("没有科目，请先添加科目\n");
		SetIEndFlag(1);
		return;
	}

	cout<<"请添加分数对象的用户名："<<endl;
	string strAccount;
	cin>>strAccount;
	if (!(CheckTool::CheckStringLen(strAccount, 30) && CheckTool::CheckStringByValid(strAccount, "A~Z|a~z|0~9")))
	{
		OperInputErrorHandle(false);
		return;
	}
	
	printf("按照从左到右和从上到下的顺序输入对应科目分数（格式为 XX|XX|XX）：\n");
	ShowSubjects(mStrShowTmp, 5);
	string strScore;
	cin>>strScore;
	if (!(CheckTool::CheckStringLen(strScore, 90) && CheckScore(strScore)))
	{
		OperInputErrorHandle(false);
		return;
	}

	CS_MSG_ADD_SINGLE_SCORE_REQ SendReq;
	SendReq.sType = 2;
	strcpy_s(SendReq.cAccount, sizeof(SendReq.cAccount), strAccount.c_str());
	strcpy_s(SendReq.sSubjectId, sizeof(SendReq.sSubjectId), StringTool::CombVecToStr(vecSubjectId).c_str());
	strcpy_s(SendReq.sScore, sizeof(SendReq.sScore), strScore.c_str());

	TCPHandle::GetInstance()->Send(&SendReq, sizeof(SendReq), /*MAIN_ID_LOGINREGISTER,*/ ASSIST_ID_ADD_SINGLE_SCORE_REQ);
}

bool AddScoreSysProc::GetSubjectsAfterAddSingleScoreRecvHandle(void* vpData, unsigned int DataLen)
{
	if (OPER_PER_ADDSINGLESCOREBYONESUBJECT != GetCurOper() && OPER_PER_ADDSINGLESCOREBYSUBJECTS != GetCurOper())
	{
		printf("不是进行该操作[%d | %d]，当前进行的操作是[%d] error\n", OPER_PER_ADDSINGLESCOREBYONESUBJECT, OPER_PER_ADDSINGLESCOREBYSUBJECTS, GetCurOper());
		SetIEndFlag(-1);
		return false;
	}
	if (DataLen != sizeof(CS_MSG_GET_SUBJECTS_ACK))
	{
		printf("DataLen[%u] error, It should be [%u]\n", DataLen, sizeof(CS_MSG_GET_SUBJECTS_ACK));
		SetIEndFlag(-1);
		return false;
	}

	CS_MSG_GET_SUBJECTS_ACK* RecvMSG = (CS_MSG_GET_SUBJECTS_ACK*) vpData;
	if (RecvMSG->bSucceed)
	{
		if (OPER_PER_ADDSINGLESCOREBYONESUBJECT == GetCurOper())
		{
			AddSingleScoreByOneSubjectChooseHandle(RecvMSG->cCanAlterSubjects);
		}
		else if(OPER_PER_ADDSINGLESCOREBYSUBJECTS == GetCurOper())
		{
			AddSingleScoreBySubjectsChooseHandle(RecvMSG->cCanAlterSubjects);
		}
		else
		{
			//printf("%s  RecvMSG->sGetType=[%d] error\n", __FUNCTION__, (int)RecvMSG->sGetType);
			SetIEndFlag(-1);
			return false;
		}

	}
	else
	{
		printf("可增删科目返回不成功");
		SetIEndFlag(-1);
		return false;
	}
	return true;
}

bool AddScoreSysProc::AddSingleScoreRecvHandle(void* vpData, unsigned int DataLen)
{
	if (OPER_PER_ADDSINGLESCOREBYONESUBJECT != GetCurOper() && OPER_PER_ADDSINGLESCOREBYSUBJECTS != GetCurOper())
	{
		printf("不是进行该操作[%d | %d]，当前进行的操作是[%d] error\n", OPER_PER_ADDSINGLESCOREBYONESUBJECT, OPER_PER_ADDSINGLESCOREBYSUBJECTS, GetCurOper());
		SetIEndFlag(-1);
		return false;
	}
	if (DataLen != sizeof(CS_MSG_ADD_SINGLE_SCORE_ACK))
	{
		printf("DataLen[%u] error, It should be [%u]\n", DataLen, sizeof(CS_MSG_ADD_SINGLE_SCORE_ACK));
		SetIEndFlag(-1);
		return false;
	}

	CS_MSG_ADD_SINGLE_SCORE_ACK* RecvMSG = (CS_MSG_ADD_SINGLE_SCORE_ACK*) vpData;
	if (RecvMSG->bSucceed)
	{
		if (RecvMSG->sType == 1)
		{
			printf("给账号[%s]增加单科分数成功\n", RecvMSG->cAccount);
			SetIEndFlag(1);
		}
		else if (RecvMSG->sType == 2)
		{
			printf("给账号[%s]增加现有所有科目分数成功\n", RecvMSG->cAccount);
			SetIEndFlag(1);
		}
		else
		{
			printf("%s  RecvMSG->sType=[%d] error\n", __FUNCTION__, (int)RecvMSG->sType);
			SetIEndFlag(-1);
			return false;
		}
	}
	else
	{
		SetIEndFlag(-1);
		printf("添加分数失败");
		return false;
	}

	return true;
}


int AddScoreSysProc::ShowSubjects(map<int, string> mIStr, int iField)
{
	if (iField == 0)
		iField = 0xEFFFFFFF;
	else if (iField < 0)
		iField *= -1;

	int iCount = 0;
	for (map<int, string>::iterator iter = mIStr.begin(); iter != mIStr.end(); iter++)
	{
		if (iCount != 0 && iCount % iField == 0)
		{
			cout<<endl;
		}
		if (iCount != 0 && iCount % iField != 0)
		{
			cout<<"\t\t";
		}
		iCount++;
		printf("%d--%s", iter->first, iter->second.c_str());
	}
	if (iCount > 0)
	{
		cout<<endl;
	}

	return iCount;
}

bool AddScoreSysProc::CheckScore(string sScore)
{
	if (sScore.empty())
	{
		return true;
	}

	vector<string> vScore = StringTool::Splite(sScore, "|");

	for (unsigned i=0; i < vScore.size(); i++)
	{	
		if (!CheckTool::CheckStringByValid(vScore.at(i), "0~9"))
		{
			return false;
		}
		int iScore = (int)atoi(vScore.at(i).c_str());
		if (iScore < 0 || iScore > 100)
		{
			return false;
		}
	}

	return true;
}