# cs16_frame_skip
* Is used: 
  https://github.com/TsudaKageyu/minhook 
* Skipping frames to increase fps
  By abusing the SCR_UpdateScreen function. This function is responsible for full rendering in the game.
  Original function: https://github.com/MoeMod/CSMoE/blob...cl_scrn.c#L497
