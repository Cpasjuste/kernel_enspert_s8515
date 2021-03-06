/*
 * arch/arm/mach-tegra/reset.h
 *
 * CPU reset dispatcher.
 *
 * Copyright (c) 2011, NVIDIA Corporation.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __MACH_TEGRA_RESET_H
#define __MACH_TEGRA_RESET_H

#define TEGRA_RESET_MASK_PRESENT	0
#define TEGRA_RESET_MASK_LP1		1
#define TEGRA_RESET_MASK_LP2		2
#define TEGRA_RESET_STARTUP_SECONDARY	3
#define TEGRA_RESET_STARTUP_LP2		4
#define TEGRA_RESET_STARTUP_LP1		5
#define TEGRA_RESET_C0_L2_TAG_LATENCY	6
#define TEGRA_RESET_C0_L2_DATA_LATENCY	7
#define TEGRA_RESET_C1_L2_TAG_LATENCY	8
#define TEGRA_RESET_C1_L2_DATA_LATENCY	9
#define TEGRA_RESET_MASK_MC_CLK		10
#define TEGRA_RESET_DATA_SIZE		11

#define RESET_DATA(x)	((TEGRA_RESET_##x)*4)

#ifndef __ASSEMBLY__

#include <linux/cpumask.h>

extern unsigned long __tegra_cpu_reset_handler_data[TEGRA_RESET_DATA_SIZE];

void __tegra_cpu_reset_handler_start(void);
void __tegra_cpu_reset_handler(void);
void __tegra_cpu_reset_handler_end(void);
void tegra_secondary_startup(void);

#ifdef CONFIG_PM_SLEEP
#define tegra_cpu_lp1_mask ((unsigned long *)(IO_ADDRESS(TEGRA_RESET_HANDLER_BASE + \
		((u32)&__tegra_cpu_reset_handler_data[TEGRA_RESET_MASK_LP1] - \
		 (u32)__tegra_cpu_reset_handler_start))))

#define tegra_mc_clk_mask ((unsigned long *)(IO_ADDRESS(TEGRA_RESET_HANDLER_BASE + \
		((u32)&__tegra_cpu_reset_handler_data[TEGRA_RESET_MASK_MC_CLK] - \
		 (u32)__tegra_cpu_reset_handler_start))))

#define tegra_cpu_reset_handler_ptr ((u32 *)(IO_ADDRESS(TEGRA_RESET_HANDLER_BASE + \
		((u32)__tegra_cpu_reset_handler_data - \
		 (u32)__tegra_cpu_reset_handler_start))))

#define tegra_cpu_lp2_mask ((cpumask_t *)(IO_ADDRESS(TEGRA_RESET_HANDLER_BASE + \
		((u32)&__tegra_cpu_reset_handler_data[TEGRA_RESET_MASK_LP2] - \
		 (u32)__tegra_cpu_reset_handler_start))))
#endif

#define tegra_cpu_c0_l2_tag_latency \
	__tegra_cpu_reset_handler_data[TEGRA_RESET_C0_L2_TAG_LATENCY]

#define tegra_cpu_c0_l2_tag_latency_iram \
	((u32 *)(IO_ADDRESS(TEGRA_RESET_HANDLER_BASE +			\
	((u32)&__tegra_cpu_reset_handler_data[TEGRA_RESET_C0_L2_TAG_LATENCY] \
	 - (u32)__tegra_cpu_reset_handler_start))))

#define tegra_cpu_c0_l2_data_latency \
	__tegra_cpu_reset_handler_data[TEGRA_RESET_C0_L2_DATA_LATENCY]

#define tegra_cpu_c0_l2_data_latency_iram \
	((u32 *)(IO_ADDRESS(TEGRA_RESET_HANDLER_BASE +			\
	((u32)&__tegra_cpu_reset_handler_data[TEGRA_RESET_C0_L2_DATA_LATENCY] \
	 - (u32)__tegra_cpu_reset_handler_start))))

#define tegra_cpu_c1_l2_tag_latency \
	__tegra_cpu_reset_handler_data[TEGRA_RESET_C1_L2_TAG_LATENCY]

#define tegra_cpu_c1_l2_tag_latency_iram \
	((u32 *)(IO_ADDRESS(TEGRA_RESET_HANDLER_BASE +			\
	((u32)&__tegra_cpu_reset_handler_data[TEGRA_RESET_C1_L2_TAG_LATENCY] \
	 - (u32)__tegra_cpu_reset_handler_start))))

#define tegra_cpu_c1_l2_data_latency \
	__tegra_cpu_reset_handler_data[TEGRA_RESET_C1_L2_DATA_LATENCY]

#define tegra_cpu_c1_l2_data_latency_iram \
	((u32 *)(IO_ADDRESS(TEGRA_RESET_HANDLER_BASE +			\
	((u32)&__tegra_cpu_reset_handler_data[TEGRA_RESET_C1_L2_DATA_LATENCY] \
	 - (u32)__tegra_cpu_reset_handler_start))))

#define tegra_cpu_reset_handler_offset \
		((u32)__tegra_cpu_reset_handler - \
		 (u32)__tegra_cpu_reset_handler_start)

#define tegra_cpu_reset_handler_size \
		(__tegra_cpu_reset_handler_end - \
		 __tegra_cpu_reset_handler_start)

void __init tegra_cpu_reset_handler_init(void);

#ifdef CONFIG_PM_SLEEP
void tegra_cpu_reset_handler_save(void);
void tegra_cpu_reset_handler_restore(void);
#endif
#endif
#endif
