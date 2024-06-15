#include <imgui.h>
#include <filesystem>
#include <map>
#include "lua.hpp"
#include "breakpoint.h"

struct FontSetting{
	std::string name;
	std::string file;
	std::string glyph_ranges_fn;
	double size_pixels = 20;
};
std::map<std::string, int> fontMap;

int SetCustomFont(lua_State* L) {
	std::string name = luaL_checkstring(L, 1);
	bool flag = false;
	if (fontMap.count(name)) {
		auto& io = ImGui::GetIO();
		flag = true;
		ImGui::PushFont(io.Fonts->Fonts[fontMap[name]]);
	}
	lua_pushboolean(L, flag);
	return 1;
}
static bplib::BreakPoint* lookNewstate = nullptr;

void lookLuaNewstate(PCONTEXT cont) {
	size_t stackPtr = cont->Esp;
	uint32_t ptr = *(uint32_t*)stackPtr;

	if (!lookNewstate->HasPoint((LPVOID)ptr)) {
		lookNewstate->SetPoint((LPVOID)ptr);
		lookNewstate->AddPointCB((LPVOID)ptr, [](PCONTEXT cont) {
			lua_State* L = (lua_State*)cont->Eax;
			lua_pushcfunction(L, SetCustomFont);
			lua_setglobal(L, "SetCustomFont");
		});
	}
}

int LoadFonts(lua_State* L) {
	static double scale = luaL_checknumber(L, 1);
	
	bplib::BreakPoint* Monitor = (bplib::BreakPoint*)lua_newuserdata(L, sizeof(bplib::BreakPoint));
	lookNewstate = Monitor;

	DWORD id = GetCurrentProcessId();
	new (Monitor) bplib::BreakPoint(id);
	union {
		bool (ImFontAtlas::* fn)() = &ImFontAtlas::Build;
		void* p;
	}ptr;
	Monitor->SetPoint((LPVOID)ptr.p);
	Monitor->AddPointCB((LPVOID)ptr.p, [](PCONTEXT cont) {
		lookNewstate->RemovePoint((LPVOID)luaL_newstate);
		lua_State* loadData = luaL_newstate();
		lookNewstate->SetPoint((LPVOID)luaL_newstate);
		lookNewstate->AddPointCB((LPVOID)luaL_newstate, lookLuaNewstate);
		auto& io = ImGui::GetIO();
		std::filesystem::directory_iterator dir_iter(std::filesystem::current_path().string() + "/mods/");
		for (const auto& entry : dir_iter) {
			if (!entry.is_directory()) {//如果不是目录，就下一个
				continue;
			}
			std::string fontListPath = entry.path().string() + "/NotaDearImGuiFont/";
			std::string SettingPath = fontListPath + "FontSetting.lua";
			if (!std::filesystem::exists(SettingPath)) {//如果不存在就下一个
				continue;
			}
			luaL_dofile(loadData, SettingPath.c_str());
			lua_getglobal(loadData, "font");
			if (lua_istable(loadData, -1)) {
				lua_pushnil(loadData); //初始化第一个索引为nil
				std::vector<FontSetting> settings;

				//从第一个索引开始遍历表
				while (lua_next(loadData, -2) != 0) {
					settings.push_back(FontSetting());
					int index = settings.size() - 1;
					//获取当前索引的值
					lua_pushnil(loadData); //指定第二个索引为nil
					while (lua_next(loadData, -2) != 0) {
						// 获取当前索引下的值
						std::string key = lua_tostring(loadData, -2);
						if (key == "size_pixels") {
							double value = lua_tonumber(loadData, -1);
							settings[index].size_pixels = value;
							//std::cout << "Font Test key:" << key << "value:" << value<<std::endl;
						}
						else {
							std::string value = lua_tostring(loadData, -1);
							if (key == "name") {
								settings[index].name = value;
							}
							else if (key == "file") {
								settings[index].file = value;
							}
							else if (key == "glyph_ranges_fn") {
								settings[index].glyph_ranges_fn = value;
							}
							//std::cout << "Font Test key:" << key << " value:" << value << std::endl;
						}
						//移除当前值，准备下一次迭代
						lua_pop(loadData, 1);
					}

					//移除当前索引，准备下一次迭代
					lua_pop(loadData, 1);
				}
				for (auto& v : settings) {
					//std::cout << "Reg name" << v.size_pixels << '\n';
					if (v.name.empty() || v.file.empty()) {//如果名字或文件为空
						continue;
					}
					const ImWchar* ranges = io.Fonts->GetGlyphRangesDefault();
					if (v.glyph_ranges_fn == "GetGlyphRangesGreek") {
						ranges = io.Fonts->GetGlyphRangesGreek();
					}
					else if (v.glyph_ranges_fn == "GetGlyphRangesKorean") {
						ranges = io.Fonts->GetGlyphRangesKorean();
					}
					else if (v.glyph_ranges_fn == "GetGlyphRangesJapanese") {
						ranges = io.Fonts->GetGlyphRangesJapanese();
					}
					else if (v.glyph_ranges_fn == "GetGlyphRangesChineseFull") {
						ranges = io.Fonts->GetGlyphRangesChineseFull();
					}
					else if (v.glyph_ranges_fn == "GetGlyphRangesChineseSimplifiedCommon") {
						ranges = io.Fonts->GetGlyphRangesChineseSimplifiedCommon();
					}
					else if (v.glyph_ranges_fn == "GetGlyphRangesCyrillic") {
						ranges = io.Fonts->GetGlyphRangesCyrillic();
					}
					else if (v.glyph_ranges_fn == "GetGlyphRangesThai") {
						ranges = io.Fonts->GetGlyphRangesThai();
					}
					else if (v.glyph_ranges_fn == "GetGlyphRangesVietnamese") {
						ranges = io.Fonts->GetGlyphRangesVietnamese();
					}
					std::string file = fontListPath + v.file;
					io.Fonts->AddFontFromFileTTF(file.c_str(), v.size_pixels * scale, nullptr, ranges);
					fontMap[v.name] = io.Fonts->Fonts.size() - 1;
				}
			}
			lua_close(loadData);
		}
	});
	lookNewstate->SetPoint((LPVOID)luaL_newstate);
	lookNewstate->AddPointCB((LPVOID)luaL_newstate, lookLuaNewstate);
	luaL_getmetatable(L, "noita_dear_imgui_font_loader");//设置元表属性
	lua_setmetatable(L, -2);
	return 1;
}
int DestroyLoadFonts(lua_State* L) {//析构函数
	bplib::BreakPoint* Monitor = (bplib::BreakPoint*)lua_touserdata(L, 1);
	Monitor->~BreakPoint();//调用析构函数，防止内存泄漏
	return 0;
}
static luaL_Reg Monitor[] = {
	{ "__gc",DestroyLoadFonts}
};

//提供给lua的函数
static luaL_Reg luaLibs[] = {
	{ "LoadFonts", LoadFonts},
	{ NULL, NULL }
};

extern "C" __declspec(dllexport)
int luaopen_Loader(lua_State* L) {
	//创建元表
	luaL_newmetatable(L, "noita_dear_imgui_font_loader");
	lua_pushvalue(L, -1);//备份副本到栈顶中

	//将元表的__index字段指向自身，然后注册函数，以实现类似调用类方法的形式
	lua_setfield(L, -2, "__index");//会执行弹出操作
	luaL_register(L, NULL, Monitor);

	luaL_register(L, "Loader", luaLibs);  //注册函数，参数2是模块名
	return 1;
}
