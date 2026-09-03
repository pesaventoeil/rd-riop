/*
 * Copyright 2025 NXP
 *
 * NXP Confidential and Proprietary. This software is owned or controlled by NXP
 * and may only be used strictly in accordance with the applicable license
 * terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you
 * have read, and that you agree to comply with and are bound by, such license
 * terms. If you do not agree to be bound by the applicable license terms, then
 * you may not retain, install, activate or otherwise use the software.
 */
#undef GOAL_CM_VAR
#undef GOAL_CM_INST_VAR
#if GOAL_CM_NAMES == 1
#define GOAL_CM_VAR(varId, type, maxSize, validate, change) {varId, type, maxSize, NULL, validate, change, NULL, NULL, #varId}
#define GOAL_CM_INST_VAR(varId, instId, offset, type, maxSize, validate, change) {varId + (instId * offset), type, maxSize, NULL, validate, change, NULL, NULL, #varId}
#else
#define GOAL_CM_VAR(varId, type, maxSize, validate, change) {varId, type, maxSize, NULL, validate, change, NULL, NULL}
#define GOAL_CM_INST_VAR(varId, instId, offset, type, maxSize, validate, change) {varId + (instId * offset), type, maxSize, NULL, validate, change, NULL, NULL}
#endif
