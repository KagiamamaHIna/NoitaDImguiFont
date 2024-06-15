package.cpath = package.cpath..";./mods/NoitaDImguiFont/?.dll"
Cpp = require("Loader") --加载模块
function noita_dear_imgui_setting_get(key)
    return ModSettingGet("NoitaDearImGui." .. key) or false
end

local scale = tonumber(noita_dear_imgui_setting_get("scale"))
if scale == nil then
    scale = 1
end

local a = Cpp.LoadFonts(scale)