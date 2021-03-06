/* linux/drivers/media/video/exynos/fimg2d/fimg2d_helper.c
 *
 * Copyright (c) 2011 Samsung Electronics Co., Ltd.
 *	http://www.samsung.com/
 *
 * Samsung Graphics 2D driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include "fimg2d.h"
#include "fimg2d_helper.h"

static char *opname(enum blit_op op)
{
	switch (op) {
	case BLIT_OP_SOLID_FILL:
		return "FILL";
	case BLIT_OP_CLR:
		return "CLR";
	case BLIT_OP_SRC:
		return "SRC";
	case BLIT_OP_DST:
		return "DST";
	case BLIT_OP_SRC_OVER:
		return "SRC_OVER";
	case BLIT_OP_DST_OVER:
		return "DST_OVER";
	case BLIT_OP_SRC_IN:
		return "SRC_IN";
	case BLIT_OP_DST_IN:
		return "DST_IN";
	case BLIT_OP_SRC_OUT:
		return "SRC_OUT";
	case BLIT_OP_DST_OUT:
		return "DST_OUT";
	case BLIT_OP_SRC_ATOP:
		return "SRC_ATOP";
	case BLIT_OP_DST_ATOP:
		return "DST_ATOP";
	case BLIT_OP_XOR:
		return "XOR";
	case BLIT_OP_ADD:
		return "ADD";
	case BLIT_OP_MULTIPLY:
		return "MULTIPLY";
	case BLIT_OP_SCREEN:
		return "SCREEN";
	case BLIT_OP_DARKEN:
		return "DARKEN";
	case BLIT_OP_LIGHTEN:
		return "LIGHTEN";
	default:
		return "CHECK";
	}
}

static char *cfname(enum color_format fmt)
{
	switch (fmt) {
	case CF_XRGB_8888:
		return "XRGB_8888";
	case CF_ARGB_8888:
		return "ARGB_8888";
	case CF_RGB_565:
		return "RGB_565";
	case CF_XRGB_1555:
		return "XRGB_1555";
	case CF_ARGB_1555:
		return "ARGB_1555";
	case CF_XRGB_4444:
		return "XRGB_4444";
	case CF_ARGB_4444:
		return "ARGB_4444";
	case CF_RGB_888:
		return "RGB_888";
	case CF_YCBCR_444:
		return "YCbCr_444";
	case CF_YCBCR_422:
		return "YCbCr_422";
	case CF_YCBCR_420:
		return "YCbCr_420";
	case CF_A8:
		return "A8";
	case CF_L8:
		return "L8";
	case CF_MSK_1BIT:
		return "MSK_1BIT";
	case CF_MSK_4BIT:
		return "MSK_4BIT";
	case CF_MSK_8BIT:
		return "MSK_8BIT";
	case CF_MSK_16BIT_565:
		return "MSK_565";
	case CF_MSK_16BIT_1555:
		return "MSK_1555";
	case CF_MSK_16BIT_4444:
		return "MSK_4444";
	case CF_MSK_32BIT_8888:
		return "MSK_8888";
	default:
		return "CHECK";
	}
}

void fimg2d_debug_command(struct fimg2d_bltcmd *cmd)
{
	int i;
	struct fimg2d_param *p = &cmd->blt.param;
	struct fimg2d_image *img;
	struct fimg2d_rect *r;

	if (WARN_ON(!cmd->ctx))
		return;

	pr_info("\n[%s] ctx: %p seq_no(%u)\n", __func__, cmd->ctx, cmd->blt.seq_no);
	pr_info(" op: %s(%d)\n", opname(cmd->blt.op), cmd->blt.op);
	pr_info(" solid color: 0x%lx\n", p->solid_color);
	pr_info(" g_alpha: 0x%x\n", p->g_alpha);
	pr_info(" premultiplied: %d\n", p->premult);
	if (p->dither)
		pr_info(" dither: %d\n", p->dither);
	if (p->rotate)
		pr_info(" rotate: %d\n", p->rotate);
	if (p->repeat.mode) {
		pr_info(" repeat: %d, pad color: 0x%lx\n",
				p->repeat.mode, p->repeat.pad_color);
	}
	if (p->bluscr.mode) {
		pr_info(" bluescreen mode: %d, bs_color: 0x%lx " \
				"bg_color: 0x%lx\n",
				p->bluscr.mode, p->bluscr.bs_color,
				p->bluscr.bg_color);
	}
	if (p->scaling.mode) {
		pr_info(" scaling %d, s:%d,%d d:%d,%d\n",
				p->scaling.mode,
				p->scaling.src_w, p->scaling.src_h,
				p->scaling.dst_w, p->scaling.dst_h);
	}
	if (p->clipping.enable) {
		pr_info(" clipping LT(%d,%d) RB(%d,%d) WH(%d,%d)\n",
				p->clipping.x1, p->clipping.y1,
				p->clipping.x2, p->clipping.y2,
				rect_w(&p->clipping), rect_h(&p->clipping));
	}

	for (i = 0; i < MAX_IMAGES; i++) {
		size_t num_planes, j;
		img = &cmd->image[i];
		r = &img->rect;

		if (!img->addr.type)
			continue;

		num_planes = fimg2d_num_planes(img);
		for (j = 0; j < num_planes; j++) {
			pr_info(" %s fd[%u]: %d\n",
					imagename(i), j, img->addr.fd[j]);
		}

		pr_info(" %s width: %d height: %d " \
				"stride: %d order: %d format: %s(%d)\n",
				imagename(i), img->width, img->height,
				img->stride, img->order,
				cfname(img->fmt), img->fmt);
		pr_info(" %s rect LT(%d,%d) RB(%d,%d) WH(%d,%d)\n",
				imagename(i), r->x1, r->y1, r->x2, r->y2,
				rect_w(r), rect_h(r));
	}
}

void fimg2d_debug_command_simple(struct fimg2d_bltcmd *cmd)
{
	struct fimg2d_blit *blt = &cmd->blt;
	struct fimg2d_image *src;
	struct fimg2d_image *dst;
	struct fimg2d_clip *clp;

	src = blt->src;
	dst = blt->dst;
	clp = &blt->param.clipping;

	if (!src) {
		pr_info("\n dst fs(%d,%d) rect(%d,%d,%d,%d)\n",
				dst->width, dst->height,
				dst->rect.x1, dst->rect.y1,
				dst->rect.x2, dst->rect.y2);
	} else if (clp->enable) {
		pr_info("\n src fs(%d,%d) rect(%d,%d,%d,%d)" \
				" dst fs(%d,%d) rect(%d,%d,%d,%d)" \
				" clip(%d,%d,%d,%d)\n",
				src->width, src->height,
				src->rect.x1, src->rect.y1,
				src->rect.x2, src->rect.y2,
				dst->width, dst->height,
				dst->rect.x1, dst->rect.y1,
				dst->rect.x2, dst->rect.y2,
				clp->x1, clp->y1, clp->x2, clp->y2);
	} else {
		pr_info("\n src fs(%d,%d) rect(%d,%d,%d,%d)" \
				" dst fs(%d,%d) rect(%d,%d,%d,%d)\n",
				src->width, src->height,
				src->rect.x1, src->rect.y1,
				src->rect.x2, src->rect.y2,
				dst->width, dst->height,
				dst->rect.x1, dst->rect.y1,
				dst->rect.x2, dst->rect.y2);
	}
}
