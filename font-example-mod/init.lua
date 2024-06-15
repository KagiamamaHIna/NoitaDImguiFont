-- ... code
--使用 void SetCustomFont(std::string name) 函数直接加载字体

--Use void SetCustomFont(std::string name) function to load the font directly.

--示例 | example
--[[
imgui = load_imgui({ version = "1.20.0", mod = "example_font" })
local font_text = "114514文本"

function OnWorldPostUpdate()
    if imgui.Begin("Fonts") then

		SetCustomFont("Big-OPPOSans")
        _, font_text = imgui.InputText("Text", font_text)
        imgui.PushFont(imgui.GetNoitaFont())
        imgui.Text("A Noita Pixel text")
		SetCustomFont("OPPOSans")
		imgui.Text("是中文！")
        imgui.End()
    end
end
]]
