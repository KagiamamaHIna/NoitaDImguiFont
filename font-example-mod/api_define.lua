---@diagnostic disable: missing-return

---通过name直接加载一个字体，不推荐使用，因为在快速重启后会失效

---Load a font directly via name, but it's not recommended because it will fail after a quick new game
---@param name string
---@return boolean
function SetCustomFont(name) end

---通过一个内部索引直接加载一个字体。推荐使用，因为不会在快速重启后失效

---Loads a font directly via an internal index. Recommended because it won't fail after a quick new game
---@param index integer
---@return boolean
function SetFontForIndex(index)end

---通过name直接获得一个字体的内部索引，快速重启后会失效，建议通过一些方法保存索引，通过SetFontForIndex直接使用索引加载，以避免问题。重新启动游戏后不保证索引的正确

---Getting the internal index of a font directly via name, but it will fail after a quick new game, it is recommended to save the index by some method and load it directly using the index via SetFontForIndex to avoid problems. Correct indexing is not guaranteed after restarting the game
---@param name string
---@return integer
function GetCustomFontIndex(name) end