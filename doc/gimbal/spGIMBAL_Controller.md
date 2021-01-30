# spGIMBAL_Controller

struct __GIMBAL_Controller_Type spGIMBAL_Controller = {
    ._system = {
        .init = [GIMBAL_ControlInit](../GIMBAL_ControlInit().md),
        .looper = [GIMBAL_ControlLooper](GIMBAL_ControlLooper().md),
        .regression = GIMBAL_MiddleLooper,
				.statelooper = GIMBAL_State
    },
    .user = {
        .update_target_pitch = GIMBAL_UpdatePitch,
        .update_target_yaw = GIMBAL_UpdateYaw,
        .update_target = GIMBAL_Update,
				.update_target_limit = GIMBAL_Update_Limit,
				.update_enemy_location = GIMBAL_Update_enemy_Location,
				.pid_init = GIMBAL_PID_Init,
				.visual_sy_pid = GIMBAL_VISUAL_SY_PID_Init,
				.visual_sp_pid = GIMBAL_VISUAL_SP_PID_Init,
				.visual_ly_pid = GIMBAL_VISUAL_LY_PID_Init,
				.visual_lp_pid = GIMBAL_VISUAL_LP_PID_Init,
				.cruise_pid_init = GIMBAL_CRUISE_PID_Init,
    }
};