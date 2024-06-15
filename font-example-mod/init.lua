--init.lua示例 | init.lua example
--[[
imgui = load_imgui({ version = "1.20.0", mod = "example_font" })
local font_text = "114514文本"

local BigfontIndex = GetCustomFontIndex("Example-Big-OPPOSans")
local fontIndex = GetCustomFontIndex("Example-OPPOSans")
if BigfontIndex then
    ModSettingSet("dear_font_big_opposans", BigfontIndex)
else
	BigfontIndex = ModSettingGet("dear_font_big_opposans")
end
if fontIndex then
    ModSettingSet("dear_font_opposans", fontIndex)
else
	fontIndex = ModSettingGet("dear_font_opposans")
end

function OnWorldPostUpdate()
    if imgui.Begin("Fonts") then

		SetFontForIndex(BigfontIndex)
        _, font_text = imgui.InputText("Text", font_text)
        imgui.PushFont(imgui.GetNoitaFont())
        imgui.Text("A Noita Pixel text")
		SetFontForIndex(fontIndex)
		imgui.Text("是中文！")
        imgui.End()
    end
end
]]
