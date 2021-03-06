#ifndef __AUTHORITYMGR_H__
#define __AUTHORITYMGR_H__

#include <iostream>
#include <vector>
#include "PublicDef.h"

using namespace std;

class AuthorityMgr
{
public:
	AuthorityMgr();
	~AuthorityMgr();

	static void GetDefaultAuthorityByIdent(IdentType Ident, vector<OperPermission>& vecOper);

};

#endif