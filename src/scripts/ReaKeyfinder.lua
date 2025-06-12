package.path = reaper.ImGui_GetBuiltinPath() .. '/?.lua'
local ImGui = require 'imgui' '0.9.3'
local ctx = ImGui.CreateContext('ReaKeyfinder')
local size_x, size_y = 420, 360

package.path = package.path .. ";" .. string.match(({reaper.get_action_context()})[2], "(.-)([^\\/]-%.?([^%.\\/]*))$") .. "?.lua"
local json = require ("lua.json")

-- Reaper data
local project_bpm = string.format("%.3f", reaper.Master_GetTempo())

-- Parse JSON
item = reaper.GetSelectedMediaItem(0,0)
take = reaper.GetMediaItemTake(item, 0)
json_str = ''
retval, json_str = reaper.GetSetMediaItemTakeInfo_String(take, "P_EXT:reakeyfinder", json_str, false)

-- Try decoding JSON safely
local success, data = pcall(json.decode, json_str)

if success and type(data) == "table" then
  -- Assign local variables
  bpm = data.bpm
  key = data.key
  key_quart = data.key_quart
  key_quint = data.key_quint
  key_relative = data.key_relative
  rkf_ver = data.rkf_ver

  -- Nested table values
  take_info_name = data.take_info.name
  take_info_pitch = data.take_info.pitch
  take_info_playrate = data.take_info.playrate

  

else
  reaper.ShowMessageBox("Main ReaKeyfinder plugin has not analyzed this item yet.", "Error", 0)
  return
end

local function loop()
  local visible, open = ImGui.Begin(ctx, 'ReaKeyfinder', true)
  if visible then
    reaper.ImGui_SetNextWindowSize(ctx, size_x, size_y, reaper.ImGui_Cond_Once())
    
        -- Take name section
        reaper.ImGui_SetCursorPos(ctx, 20, 32)
        if reaper.ImGui_BeginChild(ctx, 'take_name', 375, 62, 1) then
          reaper.ImGui_SetCursorPos(ctx, 20, 8)
          reaper.ImGui_Text(ctx, 'Take name:')
    
          reaper.ImGui_SetCursorPos(ctx, 20, 24)
          reaper.ImGui_PushItemWidth(ctx, 340)
          reaper.ImGui_BeginDisabled(ctx)
          reaper.ImGui_InputText(ctx, '##take_name', take_info_name)
          reaper.ImGui_EndDisabled(ctx)
          reaper.ImGui_PopItemWidth(ctx)
    
          reaper.ImGui_EndChild(ctx)
        end
        
        -- Key section
        reaper.ImGui_SetCursorPos(ctx, 20, 100)
            if reaper.ImGui_BeginChild(ctx, 'key_section', 375, 134, 1) then
              reaper.ImGui_SetCursorPos(ctx, 20, 8)
              reaper.ImGui_Text(ctx, 'Key:')
        
              reaper.ImGui_SetCursorPos(ctx, 20, 24)
              reaper.ImGui_PushItemWidth(ctx, 340)
              reaper.ImGui_BeginDisabled(ctx)
              reaper.ImGui_InputText(ctx, '##key', key)
              reaper.ImGui_EndDisabled(ctx)
              reaper.ImGui_PopItemWidth(ctx)
        
              reaper.ImGui_SetCursorPos(ctx, 20, 48)
              reaper.ImGui_PushItemWidth(ctx, 280)
              reaper.ImGui_BeginDisabled(ctx)
              reaper.ImGui_InputText(ctx, 'Relative', key_relative)
              reaper.ImGui_EndDisabled(ctx)
              reaper.ImGui_PopItemWidth(ctx)
        
              reaper.ImGui_SetCursorPos(ctx, 20, 72)
              reaper.ImGui_PushItemWidth(ctx, 280)
              reaper.ImGui_BeginDisabled(ctx)
              reaper.ImGui_InputText(ctx, 'Quart', key_quart)
              reaper.ImGui_EndDisabled(ctx)
              reaper.ImGui_PopItemWidth(ctx)
        
              reaper.ImGui_SetCursorPos(ctx, 20, 96)
              reaper.ImGui_PushItemWidth(ctx, 280)
              reaper.ImGui_BeginDisabled(ctx)
              reaper.ImGui_InputText(ctx, 'Quint', key_quint)
              reaper.ImGui_EndDisabled(ctx)
              reaper.ImGui_PopItemWidth(ctx)
        
              reaper.ImGui_EndChild(ctx)
            end
            
        -- BPM Section
          reaper.ImGui_SetCursorPos(ctx, 20, 250)
          if reaper.ImGui_BeginChild(ctx, 'bpm_section', 375, 96, 1) then
            reaper.ImGui_SetCursorPos(ctx, 20, 8)
            reaper.ImGui_Text(ctx, 'BPM:')
        
            -- Track BPM (read-only)
            reaper.ImGui_SetCursorPos(ctx, 20, 28)
            reaper.ImGui_PushItemWidth(ctx, 165)
            reaper.ImGui_BeginDisabled(ctx)
            reaper.ImGui_InputText(ctx, '##Track BPM', string.format("%.3f", bpm))
            reaper.ImGui_EndDisabled(ctx)
            reaper.ImGui_PopItemWidth(ctx)
        
            -- Project BPM (editable)
            reaper.ImGui_SameLine(ctx)
            reaper.ImGui_PushItemWidth(ctx, 165)
            retval, project_bpm = reaper.ImGui_InputText(ctx, '##Project BPM', project_bpm)
            reaper.ImGui_PopItemWidth(ctx)
        
            -- Buttons
            reaper.ImGui_SetCursorPos(ctx, 20, 54)
            if reaper.ImGui_Button(ctx, 'Sync to project BPM', 165) then
              if take and bpm > 0 then
                local proj_bpm = reaper.Master_GetTempo()
                local new_rate = proj_bpm / bpm
                reaper.SetMediaItemTakeInfo_Value(take, "D_PLAYRATE", new_rate)
              else
                reaper.ShowMessageBox("No active take or invalid BPM", "Error", 0)
              end
            end
        
            reaper.ImGui_SameLine(ctx)
            if reaper.ImGui_Button(ctx, 'Set as new Project BPM', 165) then
              if bpm > 0 then
                reaper.SetCurrentBPM(0, bpm, true)
                project_bpm = string.format("%.2f", bpm)
              else
                reaper.ShowMessageBox("Invalid BPM value", "Error", 0)
              end
            end
        
            reaper.ImGui_EndChild(ctx)
          end
    
    ImGui.End(ctx)
  end
  if open then
    reaper.defer(loop)
  end
end

reaper.defer(loop)
