/*
 * arch/arm/mach-tegra/tegra3_edp.c
 *
 * Copyright (c) 2011-2013, NVIDIA CORPORATION. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>

#include <mach/edp.h>
#include "fuse.h"

/*
 * Temperature step size cannot be less than 4C because of hysteresis
 * delta
 * Code assumes different temperatures for the same speedo_id /
 * regulator_cur are adjacent in the table, and higest regulator_cur
 * comes first
 */

static char __initdata tegra3x_edp_vdd_cpu_map[] = {
	0x00, 0x2f, 0x17, 0x7d, 0x73, 0x73, 0x73, 0x00,
	0x2f, 0x2d, 0x82, 0x78, 0x78, 0x78, 0x00, 0x2f,
	0x3c, 0x82, 0x78, 0x78, 0x78, 0x00, 0x2f, 0x4b,
	0x82, 0x78, 0x78, 0x78, 0x00, 0x2f, 0x55, 0x82,
	0x78, 0x78, 0x78, 0x00, 0x28, 0x17, 0x7d, 0x73,
	0x73, 0x73, 0x00, 0x28, 0x2d, 0x82, 0x78, 0x78,
	0x78, 0x00, 0x28, 0x3c, 0x82, 0x78, 0x78, 0x78,
	0x00, 0x28, 0x4b, 0x82, 0x78, 0x78, 0x73, 0x00,
	0x28, 0x55, 0x82, 0x78, 0x78, 0x69, 0x00, 0x23,
	0x17, 0x7d, 0x73, 0x73, 0x73, 0x00, 0x23, 0x2d,
	0x82, 0x78, 0x78, 0x78, 0x00, 0x23, 0x3c, 0x82,
	0x78, 0x78, 0x6e, 0x00, 0x23, 0x4b, 0x82, 0x78,
	0x78, 0x64, 0x00, 0x23, 0x55, 0x82, 0x78, 0x6e,
	0x5a, 0x00, 0x1e, 0x17, 0x7d, 0x73, 0x73, 0x64,
	0x00, 0x1e, 0x2d, 0x82, 0x78, 0x78, 0x69, 0x00,
	0x1e, 0x3c, 0x82, 0x78, 0x78, 0x64, 0x00, 0x1e,
	0x4b, 0x82, 0x78, 0x6e, 0x5a, 0x00, 0x1e, 0x55,
	0x82, 0x78, 0x64, 0x50, 0x00, 0x19, 0x17, 0x7d,
	0x73, 0x69, 0x55, 0x00, 0x19, 0x2d, 0x82, 0x78,
	0x6e, 0x5a, 0x00, 0x19, 0x3c, 0x82, 0x78, 0x69,
	0x55, 0x00, 0x19, 0x4b, 0x82, 0x78, 0x5f, 0x4b,
	0x00, 0x19, 0x55, 0x82, 0x73, 0x55, 0x3c, 0x01,
	0x2f, 0x17, 0x7d, 0x73, 0x73, 0x73, 0x01, 0x2f,
	0x2d, 0x82, 0x78, 0x78, 0x78, 0x01, 0x2f, 0x3c,
	0x82, 0x78, 0x78, 0x78, 0x01, 0x2f, 0x4b, 0x82,
	0x78, 0x78, 0x78, 0x01, 0x2f, 0x55, 0x82, 0x78,
	0x78, 0x78, 0x01, 0x28, 0x17, 0x7d, 0x73, 0x73,
	0x73, 0x01, 0x28, 0x2d, 0x82, 0x78, 0x78, 0x78,
	0x01, 0x28, 0x3c, 0x82, 0x78, 0x78, 0x78, 0x01,
	0x28, 0x4b, 0x82, 0x78, 0x78, 0x73, 0x01, 0x28,
	0x55, 0x82, 0x78, 0x78, 0x69, 0x01, 0x23, 0x17,
	0x7d, 0x73, 0x73, 0x73, 0x01, 0x23, 0x2d, 0x82,
	0x78, 0x78, 0x78, 0x01, 0x23, 0x3c, 0x82, 0x78,
	0x78, 0x6e, 0x01, 0x23, 0x4b, 0x82, 0x78, 0x78,
	0x64, 0x01, 0x23, 0x55, 0x82, 0x78, 0x6e, 0x5a,
	0x01, 0x1e, 0x17, 0x7d, 0x73, 0x73, 0x64, 0x01,
	0x1e, 0x2d, 0x82, 0x78, 0x78, 0x69, 0x01, 0x1e,
	0x3c, 0x82, 0x78, 0x78, 0x64, 0x01, 0x1e, 0x4b,
	0x82, 0x78, 0x6e, 0x5a, 0x01, 0x1e, 0x55, 0x82,
	0x78, 0x64, 0x50, 0x01, 0x19, 0x17, 0x7d, 0x73,
	0x69, 0x55, 0x01, 0x19, 0x2d, 0x82, 0x78, 0x6e,
	0x5a, 0x01, 0x19, 0x3c, 0x82, 0x78, 0x69, 0x55,
	0x01, 0x19, 0x4b, 0x82, 0x78, 0x5f, 0x4b, 0x01,
	0x19, 0x55, 0x82, 0x73, 0x55, 0x3c, 0x02, 0x3d,
	0x17, 0x87, 0x7d, 0x7d, 0x7d, 0x02, 0x3d, 0x2d,
	0x8c, 0x82, 0x82, 0x82, 0x02, 0x3d, 0x3c, 0x8c,
	0x82, 0x82, 0x82, 0x02, 0x3d, 0x4b, 0x8c, 0x82,
	0x82, 0x82, 0x02, 0x3d, 0x55, 0x8c, 0x82, 0x82,
	0x82, 0x02, 0x32, 0x17, 0x87, 0x7d, 0x7d, 0x7d,
	0x02, 0x32, 0x2d, 0x8c, 0x82, 0x82, 0x82, 0x02,
	0x32, 0x3c, 0x8c, 0x82, 0x82, 0x82, 0x02, 0x32,
	0x4b, 0x8c, 0x82, 0x82, 0x78, 0x02, 0x32, 0x55,
	0x8c, 0x82, 0x82, 0x6e, 0x02, 0x28, 0x17, 0x87,
	0x7d, 0x7d, 0x73, 0x02, 0x28, 0x2d, 0x8c, 0x82,
	0x82, 0x78, 0x02, 0x28, 0x3c, 0x8c, 0x82, 0x82,
	0x73, 0x02, 0x28, 0x4b, 0x8c, 0x82, 0x78, 0x69,
	0x02, 0x28, 0x55, 0x8c, 0x82, 0x6e, 0x5a, 0x02,
	0x23, 0x17, 0x87, 0x7d, 0x7d, 0x69, 0x02, 0x23,
	0x2d, 0x8c, 0x82, 0x82, 0x6e, 0x02, 0x23, 0x3c,
	0x8c, 0x82, 0x78, 0x69, 0x02, 0x23, 0x4b, 0x8c,
	0x82, 0x6e, 0x5a, 0x02, 0x23, 0x55, 0x8c, 0x82,
	0x64, 0x50, 0x03, 0x3d, 0x17, 0x87, 0x7d, 0x7d,
	0x7d, 0x03, 0x3d, 0x2d, 0x8c, 0x82, 0x82, 0x82,
	0x03, 0x3d, 0x3c, 0x8c, 0x82, 0x82, 0x82, 0x03,
	0x3d, 0x4b, 0x8c, 0x82, 0x82, 0x82, 0x03, 0x3d,
	0x55, 0x8c, 0x82, 0x82, 0x82, 0x03, 0x32, 0x17,
	0x87, 0x7d, 0x7d, 0x7d, 0x03, 0x32, 0x2d, 0x8c,
	0x82, 0x82, 0x82, 0x03, 0x32, 0x3c, 0x8c, 0x82,
	0x82, 0x82, 0x03, 0x32, 0x4b, 0x8c, 0x82, 0x82,
	0x78, 0x03, 0x32, 0x55, 0x8c, 0x82, 0x82, 0x6e,
	0x03, 0x28, 0x17, 0x87, 0x7d, 0x7d, 0x73, 0x03,
	0x28, 0x2d, 0x8c, 0x82, 0x82, 0x78, 0x03, 0x28,
	0x3c, 0x8c, 0x82, 0x82, 0x73, 0x03, 0x28, 0x4b,
	0x8c, 0x82, 0x78, 0x69, 0x03, 0x28, 0x55, 0x8c,
	0x82, 0x6e, 0x5a, 0x03, 0x23, 0x17, 0x87, 0x7d,
	0x7d, 0x69, 0x03, 0x23, 0x2d, 0x8c, 0x82, 0x82,
	0x6e, 0x03, 0x23, 0x3c, 0x8c, 0x82, 0x78, 0x69,
	0x03, 0x23, 0x4b, 0x8c, 0x82, 0x6e, 0x5a, 0x03,
	0x23, 0x55, 0x8c, 0x82, 0x64, 0x50, 0x04, 0x32,
	0x17, 0x91, 0x87, 0x87, 0x87, 0x04, 0x32, 0x2d,
	0x96, 0x8c, 0x8c, 0x8c, 0x04, 0x32, 0x3c, 0x96,
	0x8c, 0x8c, 0x8c, 0x04, 0x32, 0x46, 0x96, 0x8c,
	0x8c, 0x8c, 0x04, 0x32, 0x4b, 0x82, 0x78, 0x78,
	0x78, 0x04, 0x32, 0x55, 0x82, 0x78, 0x78, 0x78,
	0x04, 0x2f, 0x17, 0x91, 0x87, 0x87, 0x87, 0x04,
	0x2f, 0x2d, 0x96, 0x8c, 0x8c, 0x8c, 0x04, 0x2f,
	0x3c, 0x96, 0x8c, 0x8c, 0x8c, 0x04, 0x2f, 0x46,
	0x96, 0x8c, 0x8c, 0x82, 0x04, 0x2f, 0x4b, 0x82,
	0x78, 0x78, 0x78, 0x04, 0x2f, 0x55, 0x82, 0x78,
	0x78, 0x78, 0x04, 0x28, 0x17, 0x91, 0x87, 0x87,
	0x87, 0x04, 0x28, 0x2d, 0x96, 0x8c, 0x8c, 0x82,
	0x04, 0x28, 0x3c, 0x96, 0x8c, 0x8c, 0x82, 0x04,
	0x28, 0x46, 0x96, 0x8c, 0x8c, 0x78, 0x04, 0x28,
	0x4b, 0x82, 0x78, 0x78, 0x78, 0x04, 0x28, 0x55,
	0x82, 0x78, 0x78, 0x6e, 0x04, 0x23, 0x17, 0x91,
	0x87, 0x87, 0x73, 0x04, 0x23, 0x2d, 0x96, 0x8c,
	0x8c, 0x78, 0x04, 0x23, 0x3c, 0x96, 0x8c, 0x82,
	0x78, 0x04, 0x23, 0x46, 0x96, 0x8c, 0x82, 0x6e,
	0x04, 0x23, 0x4b, 0x82, 0x78, 0x78, 0x6e, 0x04,
	0x23, 0x55, 0x82, 0x78, 0x78, 0x64, 0x04, 0x1e,
	0x17, 0x91, 0x87, 0x7d, 0x69, 0x04, 0x1e, 0x2d,
	0x96, 0x8c, 0x82, 0x6e, 0x04, 0x1e, 0x3c, 0x96,
	0x8c, 0x78, 0x64, 0x04, 0x1e, 0x46, 0x96, 0x8c,
	0x78, 0x5a, 0x04, 0x1e, 0x4b, 0x82, 0x78, 0x78,
	0x5a, 0x04, 0x1e, 0x55, 0x82, 0x78, 0x64, 0x50,
	0x04, 0x19, 0x17, 0x91, 0x87, 0x69, 0x55, 0x04,
	0x19, 0x2d, 0x96, 0x8c, 0x6e, 0x5a, 0x04, 0x19,
	0x3c, 0x96, 0x82, 0x6e, 0x55, 0x04, 0x19, 0x46,
	0x96, 0x82, 0x64, 0x50, 0x04, 0x19, 0x4b, 0x82,
	0x78, 0x64, 0x50, 0x04, 0x19, 0x55, 0x82, 0x78,
	0x55, 0x3c, 0x05, 0x64, 0x17, 0xa5, 0x9b, 0x9b,
	0x9b, 0x05, 0x64, 0x2d, 0xaa, 0xa0, 0xa0, 0xa0,
	0x05, 0x64, 0x3c, 0xaa, 0xa0, 0xa0, 0xa0, 0x05,
	0x64, 0x46, 0xaa, 0xa0, 0xa0, 0xa0, 0x05, 0x64,
	0x4b, 0x8c, 0x82, 0x82, 0x82, 0x05, 0x64, 0x55,
	0x8c, 0x82, 0x82, 0x82, 0x05, 0x50, 0x17, 0xa5,
	0x9b, 0x9b, 0x9b, 0x05, 0x50, 0x2d, 0xaa, 0xa0,
	0xa0, 0xa0, 0x05, 0x50, 0x3c, 0xaa, 0xa0, 0xa0,
	0x96, 0x05, 0x50, 0x46, 0xaa, 0xa0, 0xa0, 0x96,
	0x05, 0x50, 0x4b, 0x8c, 0x82, 0x82, 0x82, 0x05,
	0x50, 0x55, 0x8c, 0x82, 0x82, 0x82, 0x05, 0x3c,
	0x17, 0xa5, 0x9b, 0x9b, 0x87, 0x05, 0x3c, 0x2d,
	0xaa, 0xa0, 0xa0, 0x8c, 0x05, 0x3c, 0x3c, 0xaa,
	0xa0, 0x96, 0x82, 0x05, 0x3c, 0x46, 0xaa, 0xa0,
	0x96, 0x78, 0x05, 0x3c, 0x4b, 0x8c, 0x82, 0x82,
	0x78, 0x05, 0x3c, 0x55, 0x8c, 0x82, 0x82, 0x6e,
	0x05, 0x28, 0x17, 0xa5, 0x91, 0x7d, 0x69, 0x05,
	0x28, 0x2d, 0xaa, 0x96, 0x82, 0x6e, 0x05, 0x28,
	0x3c, 0xaa, 0x96, 0x78, 0x64, 0x05, 0x28, 0x46,
	0xaa, 0x8c, 0x6e, 0x5a, 0x05, 0x28, 0x4b, 0x8c,
	0x82, 0x6e, 0x5a, 0x05, 0x28, 0x55, 0x8c, 0x82,
	0x64, 0x50, 0x06, 0x3d, 0x17, 0xa5, 0x9b, 0x7d,
	0x7d, 0x06, 0x3d, 0x2d, 0xaa, 0xa0, 0x82, 0x82,
	0x06, 0x3d, 0x3c, 0xaa, 0xa0, 0x82, 0x82, 0x06,
	0x3d, 0x46, 0xaa, 0xa0, 0x82, 0x82, 0x06, 0x3d,
	0x4b, 0x8c, 0x82, 0x82, 0x82, 0x06, 0x3d, 0x55,
	0x8c, 0x82, 0x82, 0x82, 0x06, 0x32, 0x17, 0xa5,
	0x9b, 0x7d, 0x7d, 0x06, 0x32, 0x2d, 0xaa, 0xa0,
	0x82, 0x82, 0x06, 0x32, 0x3c, 0xaa, 0xa0, 0x82,
	0x82, 0x06, 0x32, 0x46, 0xaa, 0xa0, 0x82, 0x78,
	0x06, 0x32, 0x4b, 0x8c, 0x82, 0x82, 0x78, 0x06,
	0x32, 0x55, 0x8c, 0x82, 0x82, 0x6e, 0x06, 0x28,
	0x17, 0xa5, 0x9b, 0x7d, 0x73, 0x06, 0x28, 0x2d,
	0xaa, 0xa0, 0x82, 0x78, 0x06, 0x28, 0x3c, 0xaa,
	0x96, 0x82, 0x73, 0x06, 0x28, 0x46, 0xaa, 0x96,
	0x78, 0x69, 0x06, 0x28, 0x4b, 0x8c, 0x82, 0x78,
	0x69, 0x06, 0x28, 0x55, 0x8c, 0x82, 0x6e, 0x5a,
	0x06, 0x23, 0x17, 0xa5, 0x91, 0x7d, 0x69, 0x06,
	0x23, 0x2d, 0xaa, 0x96, 0x82, 0x6e, 0x06, 0x23,
	0x3c, 0xaa, 0x96, 0x78, 0x69, 0x06, 0x23, 0x46,
	0xaa, 0x8c, 0x6e, 0x5a, 0x06, 0x23, 0x4b, 0x8c,
	0x82, 0x6e, 0x5a, 0x06, 0x23, 0x55, 0x8c, 0x82,
	0x64, 0x50, 0x07, 0x3b, 0x17, 0x7d, 0x73, 0x73,
	0x73, 0x07, 0x3b, 0x2d, 0x82, 0x78, 0x78, 0x78,
	0x07, 0x3b, 0x3c, 0x82, 0x78, 0x78, 0x78, 0x07,
	0x3b, 0x4b, 0x82, 0x78, 0x78, 0x78, 0x07, 0x3b,
	0x5a, 0x82, 0x78, 0x78, 0x78, 0x07, 0x32, 0x17,
	0x7d, 0x73, 0x73, 0x73, 0x07, 0x32, 0x2d, 0x82,
	0x78, 0x78, 0x78, 0x07, 0x32, 0x3c, 0x82, 0x78,
	0x78, 0x78, 0x07, 0x32, 0x4b, 0x82, 0x78, 0x78,
	0x78, 0x07, 0x32, 0x5a, 0x82, 0x78, 0x6e, 0x64,
	0x07, 0x28, 0x17, 0x7d, 0x73, 0x73, 0x69, 0x07,
	0x28, 0x2d, 0x82, 0x78, 0x78, 0x6e, 0x07, 0x28,
	0x3c, 0x82, 0x78, 0x78, 0x64, 0x07, 0x28, 0x4b,
	0x82, 0x78, 0x78, 0x64, 0x07, 0x28, 0x5a, 0x82,
	0x78, 0x64, 0x50, 0x07, 0x23, 0x17, 0x7d, 0x73,
	0x73, 0x5f, 0x07, 0x23, 0x2d, 0x82, 0x78, 0x78,
	0x64, 0x07, 0x23, 0x3c, 0x82, 0x78, 0x78, 0x64,
	0x07, 0x23, 0x4b, 0x82, 0x78, 0x64, 0x50, 0x07,
	0x23, 0x5a, 0x82, 0x78, 0x5a, 0x46, 0x08, 0x3b,
	0x17, 0x7d, 0x73, 0x73, 0x73, 0x08, 0x3b, 0x2d,
	0x82, 0x78, 0x78, 0x78, 0x08, 0x3b, 0x3c, 0x82,
	0x78, 0x78, 0x78, 0x08, 0x3b, 0x4b, 0x82, 0x78,
	0x78, 0x78, 0x08, 0x3b, 0x5a, 0x82, 0x78, 0x78,
	0x78, 0x08, 0x32, 0x17, 0x7d, 0x73, 0x73, 0x73,
	0x08, 0x32, 0x2d, 0x82, 0x78, 0x78, 0x78, 0x08,
	0x32, 0x3c, 0x82, 0x78, 0x78, 0x78, 0x08, 0x32,
	0x4b, 0x82, 0x78, 0x78, 0x78, 0x08, 0x32, 0x5a,
	0x82, 0x78, 0x6e, 0x64, 0x08, 0x28, 0x17, 0x7d,
	0x73, 0x73, 0x69, 0x08, 0x28, 0x2d, 0x82, 0x78,
	0x78, 0x6e, 0x08, 0x28, 0x3c, 0x82, 0x78, 0x78,
	0x64, 0x08, 0x28, 0x4b, 0x82, 0x78, 0x78, 0x64,
	0x08, 0x28, 0x5a, 0x82, 0x78, 0x64, 0x50, 0x08,
	0x23, 0x17, 0x7d, 0x73, 0x73, 0x5f, 0x08, 0x23,
	0x2d, 0x82, 0x78, 0x78, 0x64, 0x08, 0x23, 0x3c,
	0x82, 0x78, 0x78, 0x64, 0x08, 0x23, 0x4b, 0x82,
	0x78, 0x64, 0x50, 0x08, 0x23, 0x5a, 0x82, 0x78,
	0x5a, 0x46, 0x0c, 0x52, 0x17, 0xa5, 0x9b, 0x9b,
	0x9b, 0x0c, 0x52, 0x2d, 0xaa, 0xa0, 0xa0, 0xa0,
	0x0c, 0x52, 0x3c, 0xaa, 0xa0, 0xa0, 0xa0, 0x0c,
	0x52, 0x46, 0xaa, 0xa0, 0xa0, 0xa0, 0x0c, 0x52,
	0x4b, 0x8c, 0x82, 0x82, 0x82, 0x0c, 0x52, 0x55,
	0x8c, 0x82, 0x82, 0x82, 0x0c, 0x42, 0x17, 0xa5,
	0x9b, 0x9b, 0x91, 0x0c, 0x42, 0x2d, 0xaa, 0xa0,
	0xa0, 0x96, 0x0c, 0x42, 0x3c, 0xaa, 0xa0, 0xa0,
	0x96, 0x0c, 0x42, 0x46, 0xaa, 0xa0, 0xa0, 0x96,
	0x0c, 0x42, 0x4b, 0x8c, 0x82, 0x82, 0x82, 0x0c,
	0x42, 0x55, 0x8c, 0x82, 0x82, 0x82, 0x0c, 0x3d,
	0x17, 0xa5, 0x9b, 0x9b, 0x91, 0x0c, 0x3d, 0x2d,
	0xaa, 0xa0, 0xa0, 0x96, 0x0c, 0x3d, 0x3c, 0xaa,
	0xa0, 0xa0, 0x8c, 0x0c, 0x3d, 0x46, 0xaa, 0xa0,
	0x96, 0x8c, 0x0c, 0x3d, 0x4b, 0x8c, 0x82, 0x82,
	0x82, 0x0c, 0x3d, 0x55, 0x8c, 0x82, 0x82, 0x82,
	0x0c, 0x32, 0x17, 0xa5, 0x9b, 0x91, 0x87, 0x0c,
	0x32, 0x2d, 0xaa, 0xa0, 0x96, 0x8c, 0x0c, 0x32,
	0x3c, 0xaa, 0xa0, 0x96, 0x82, 0x0c, 0x32, 0x46,
	0xaa, 0xa0, 0x8c, 0x78, 0x0c, 0x32, 0x4b, 0x8c,
	0x82, 0x82, 0x78, 0x0c, 0x32, 0x55, 0x8c, 0x82,
	0x82, 0x6e, 0x0c, 0x28, 0x17, 0xa5, 0x9b, 0x87,
	0x73, 0x0c, 0x28, 0x2d, 0xaa, 0xa0, 0x8c, 0x78,
	0x0c, 0x28, 0x3c, 0xaa, 0x96, 0x82, 0x73, 0x0c,
	0x28, 0x46, 0xaa, 0x96, 0x78, 0x69, 0x0c, 0x28,
	0x4b, 0x8c, 0x82, 0x78, 0x69, 0x0c, 0x28, 0x55,
	0x8c, 0x82, 0x6e, 0x5a, 0x0c, 0x23, 0x17, 0xa5,
	0x91, 0x7d, 0x69, 0x0c, 0x23, 0x2d, 0xaa, 0x96,
	0x82, 0x6e, 0x0c, 0x23, 0x3c, 0xaa, 0x96, 0x78,
	0x69, 0x0c, 0x23, 0x46, 0xaa, 0x8c, 0x6e, 0x5a,
	0x0c, 0x23, 0x4b, 0x8c, 0x82, 0x6e, 0x5a, 0x0c,
	0x23, 0x55, 0x8c, 0x82, 0x64, 0x50, 0x0d, 0x64,
	0x17, 0xa5, 0x9b, 0x9b, 0x9b, 0x0d, 0x64, 0x2d,
	0xaa, 0xa0, 0xa0, 0xa0, 0x0d, 0x64, 0x3c, 0xaa,
	0xa0, 0xa0, 0xa0, 0x0d, 0x64, 0x46, 0xaa, 0xa0,
	0xa0, 0xa0, 0x0d, 0x64, 0x4b, 0x8c, 0x82, 0x82,
	0x82, 0x0d, 0x64, 0x55, 0x8c, 0x82, 0x82, 0x82,
	0x0d, 0x50, 0x17, 0xa5, 0x9b, 0x9b, 0x9b, 0x0d,
	0x50, 0x2d, 0xaa, 0xa0, 0xa0, 0xa0, 0x0d, 0x50,
	0x3c, 0xaa, 0xa0, 0xa0, 0x96, 0x0d, 0x50, 0x46,
	0xaa, 0xa0, 0xa0, 0x96, 0x0d, 0x50, 0x4b, 0x8c,
	0x82, 0x82, 0x82, 0x0d, 0x50, 0x55, 0x8c, 0x82,
	0x82, 0x82, 0x0d, 0x3c, 0x17, 0xa5, 0x9b, 0x9b,
	0x87, 0x0d, 0x3c, 0x2d, 0xaa, 0xa0, 0xa0, 0x8c,
	0x0d, 0x3c, 0x3c, 0xaa, 0xa0, 0x96, 0x82, 0x0d,
	0x3c, 0x46, 0xaa, 0xa0, 0x96, 0x78, 0x0d, 0x3c,
	0x4b, 0x8c, 0x82, 0x82, 0x78, 0x0d, 0x3c, 0x55,
	0x8c, 0x82, 0x82, 0x6e, 0x0d, 0x28, 0x17, 0xa5,
	0x91, 0x7d, 0x69, 0x0d, 0x28, 0x2d, 0xaa, 0x96,
	0x82, 0x6e, 0x0d, 0x28, 0x3c, 0xaa, 0x96, 0x78,
	0x64, 0x0d, 0x28, 0x46, 0xaa, 0x8c, 0x6e, 0x5a,
	0x0d, 0x28, 0x4b, 0x8c, 0x82, 0x6e, 0x5a, 0x0d,
	0x28, 0x55, 0x8c, 0x82, 0x64, 0x50,
};

__init struct tegra_edp_vdd_cpu_entry *tegra3x_get_vdd_cpu_map(int *sz)
{
	if (sz)
		*sz = sizeof(tegra3x_edp_vdd_cpu_map)
			/ sizeof(struct tegra_edp_vdd_cpu_entry);
	return (struct tegra_edp_vdd_cpu_entry *)tegra3x_edp_vdd_cpu_map;
}


static struct tegra_system_edp_entry __initdata tegra3x_system_edp_map[] = {

/* {SKU, power-limit (in 100mW), {freq limits (in 10Mhz)} } */

	{  1,  49, {130, 120, 120, 120} },
	{  1,  44, {130, 120, 120, 110} },
	{  1,  37, {130, 120, 110, 100} },
	{  1,  35, {130, 120, 110,  90} },
	{  1,  29, {130, 120, 100,  80} },
	{  1,  27, {130, 120,  90,  80} },
	{  1,  25, {130, 110,  80,  60} },
	{  1,  21, {130, 100,  80,  40} },

	{  4,  49, {130, 120, 120, 120} },
	{  4,  44, {130, 120, 120, 110} },
	{  4,  37, {130, 120, 110, 100} },
	{  4,  35, {130, 120, 110,  90} },
	{  4,  29, {130, 120, 100,  80} },
	{  4,  27, {130, 120,  90,  80} },
	{  4,  25, {130, 110,  80,  60} },
	{  4,  21, {130, 100,  80,  40} },
};

__init struct tegra_system_edp_entry *tegra3x_get_system_edp_map(int *sz)
{
	if (sz)
		*sz = ARRAY_SIZE(tegra3x_system_edp_map);
	return (struct tegra_system_edp_entry *)tegra3x_system_edp_map;
}
