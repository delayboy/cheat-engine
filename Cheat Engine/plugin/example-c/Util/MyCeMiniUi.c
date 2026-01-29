#include "MyCeMiniUi.h"
PVOID mainForm = NULL;
PVOID textEdit = NULL;
TCustomTreeView* mainTreeView = NULL;
HANDLE eventSignal = NULL;
char ret_str[2048];
std::string stackToStr(std::vector<int> stack) {
	std::string ret = "[";
	for (int i = 0; i < stack.size(); i++) {
		ret = ret.append(std::to_string(i)).append(",");
	}
	if (stack.size() > 0) ret.pop_back();
	ret = ret.append("]");
	return ret;

}
bool is_right_register(CECONTEXT context) {
	DWORD64* array_r = &context.Rax;
	for (int i = 0; i < 8; i++) {
		if (array_r[i] == 0x40) return true;
	}
	return false;
}
std::string int2hex(ULONG_PTR input) {
	char temp[50] = "";
	sprintf_s(temp, 50, "0x%llx", input);
	return std::string(temp);
}
void __stdcall showTreeNodeButton(TCustomTreeView* sender)
{
	TTreeNode* selected_node=luaTreeViewSelected(sender);
	if (selected_node !=NULL && selected_node->Data!=NULL) {
		CECONTEXT context = selected_node->Data->context;
		DWORD64* array_r = &context.Rax;
		std::vector<std::string> reg_name_list;
		reg_name_list.push_back("Rax");
		reg_name_list.push_back("Rcx");
		reg_name_list.push_back("Rdx");
		reg_name_list.push_back("Rbx");
		reg_name_list.push_back("Rsp");
		reg_name_list.push_back("Rbp");
		reg_name_list.push_back("Rsi");
		reg_name_list.push_back("Rdi");
		std::string res = "";
		for (int i = 0; i < 8; i++) {
			res = res.append(reg_name_list[i]).append("=").append(int2hex(array_r[i])).append("\n");

		}
		res = res.append("Rip=").append(int2hex(context.Rip));
		Exported.control_setCaption(textEdit, res.c_str());
	}
	
}

void enumAllTTreeNode(FILE* fp,TTreeNode* node) {
	std::vector<int> stack;
	stack.push_back(-1);
	
	while (stack.size() > 0)
	{
		int peek_index = stack[stack.size() - 1] + 1;
		stack.pop_back();
		if (peek_index == 0 && node->Data && node->ftext) {
			if (strstr(node->Data->instruction, "and") != NULL&& is_right_register(node->Data->context)) {
				
				std::string content;
				content = content.append(node->ftext).append("-(").append(std::to_string(node->Count)).append(")-").append(stackToStr(stack));
				if (fp) {
					fprintf(fp, "%s\t-\t0x%llx\t-\t%s\n", node->ftext, node->Data->context.Rip, content.c_str());
				}
				
				TTreeNode* new_ele=luaTreeNodesAdd(luaTreeViewItems(mainTreeView), content.c_str());
				new_ele->Data = node->Data;
				//Exported.control_onClick(new_ele, showTreeNodeButton);

			}
		
		}
		if (peek_index < node->Count) {
			stack.push_back(peek_index);
			stack.push_back(-1);
			node = node->Items[peek_index];

		}
		else {
			node = node->parent;
		}
	}
	
}
void __stdcall showClickButton()
{
	PVOID userData = luaGetTraceFile();
	if (userData == NULL) {
		luaCloseForm(mainForm);
		return;
	}
	TCustomTreeView* treeView = (TCustomTreeView*)userData;

	TTreeNodes* treeNode = luaTreeViewItems(treeView);
	FILE* fp = generateFileStream("C:/traceOutput.txt");
	for (int i = 0; i < treeNode->FTopLvlCount; i++)
	{

		TTreeNode* node = treeNode->Item[i];
		//luaPrintf("TTreeNode.Data:0x%llx", (__int64)node->Data);
		enumAllTTreeNode(fp,node);


		//luaPrintf("TTreeNode.Data.iN:%s", node->data->instruction);
		//luaLoadString("print(string.format('CETraceFile.Data = %s',CETraceFile.Items.Item[0].Data))");

	}
	luaTreeNodesAdd(treeNode, "enumAllTTreeNode over");
	if (fp)fclose(fp);
}
void __stdcall mainMenuOnClose()
{
	Exported.control_getCaption(textEdit, ret_str, 2048);
	mainForm = NULL;
	mainTreeView = NULL;
	textEdit = NULL;
	SetEvent(eventSignal);
	

}
void waitForFormClose() {
	if (eventSignal) {
		ResetEvent(eventSignal);
		bool is_waiting = true;
		MSG msg;
		while (is_waiting) {
			WORD result = MsgWaitForMultipleObjects(1, &eventSignal, FALSE, INFINITE, QS_ALLINPUT);//这个方法可以同时等待消息和信号，避免频繁轮询，提高性能
			if (result == WAIT_OBJECT_0) {
				is_waiting = false;
			}
			else if (result == WAIT_OBJECT_0 + 1) {
				while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}

	}
}

const char* InitMyCeMiniUi(const char* ini_text,bool wait_close)
{
	if (mainForm) {
		luaShowForm(mainForm);
		Exported.control_setCaption(textEdit, ini_text); //	Exported.ShowMessage("Called from lua");
		if(wait_close)waitForFormClose();
		return ret_str;
	}
	if (eventSignal == NULL) {
		eventSignal = CreateEventA(NULL, TRUE, FALSE, NULL);

	}
	//创建插件主窗口
	mainForm = luaCreateForm(true);
	Exported.form_onClose(mainForm, mainMenuOnClose);
	Exported.form_centerScreen(mainForm);
	Exported.control_setSize(mainForm, 800, 600);
	mainTreeView = (TCustomTreeView*)luaCreateTreeView(mainForm);
	Exported.control_onClick(mainTreeView, showTreeNodeButton);
	TTreeNodes* item = luaTreeViewItems(mainTreeView);
	//luaPrintf("real TTreeNodes: 0x%llx  mainTreeView.Items: 0x%llx , open: 0x%llx", (__int64)item, (__int64)mainTreeView->Items, (__int64)mainTreeView->OpenSourceUnkown);

	TTreeNode* node = luaTreeNodesAdd(luaTreeViewItems(mainTreeView), "test");
	node = luaTreeNodesGetItem(luaTreeViewItems(mainTreeView), 0);
	Exported.control_setAlign(mainTreeView, Right);
	Exported.control_setSize(mainTreeView, 700, 300);

	/*luaPrintf("real item[0] = 0x%llx ", (__int64)node);
	luaPrintf("treeNodes = 0x%llx ", (__int64)mainTreeView->Items);
	luaPrintf("item[0] = 0x%llx ", (__int64)mainTreeView->Items->Item[0]);
	luaLoadString("print(string.format('TreeNode.Data = %s',TreeNode.Data))");
	luaPrintf("item[0].data = 0x%llx ", (__int64)mainTreeView->Items->Item[0]->Data);*/

	textEdit = Exported.createMemo(mainForm);
	Exported.control_setSize(textEdit, 300, 300);
	Exported.control_setAlign(textEdit, Top);
	Exported.control_setCaption(mainForm, "my plugin ui");

	PVOID label = Exported.createLabel(mainForm);
	Exported.control_setAlign(label, Left);
	Exported.control_setCaption(label, "my main");
	PVOID button = Exported.createButton(mainForm);

	Exported.control_setAlign(button, Bottom);
	Exported.control_onClick(button, showClickButton);
	Exported.control_setCaption(button, "clk");


	Exported.control_setCaption(textEdit, ini_text); //	Exported.ShowMessage("Called from lua");

	if (wait_close)waitForFormClose();
	return ret_str;
}


int lua_plugin_print(lua_State* L) //make sure this is cdecl
{
	int type = lua_type(L, -1);
	const char* text;
	if (type == LUA_TSTRING) {//nil
		text = lua_tostring(L, -1);
		
	}
	else {
		text = lua_tostring(L, -2);
	}
	

	
	
	
	if (type == LUA_TSTRING) {
		InitMyCeMiniUi(text, false);
		lua_pushstring(L, "");
	}
	else {
		InitMyCeMiniUi(text,true);
		lua_pushstring(L, ret_str);
	}
	

	return 1;
}

