#include "StdAfx.h"
#include "Algorithm.h"

using namespace UTS::Algorithm::RectangleChart;

namespace UTS
{
    namespace Algorithm
    {
        namespace DFOV
        {
            namespace DFOV_Rectangle
            {
				#define POWER(x) ((x)*(x))

				void CaculateFOV(
					FOV_PARAM *param,
					FOV_RESULT *res)
				{
					double cross_dist_pixels_w
						= Math::GetDistance2D(
						(float)param->ptCross_hair[CrossHair::Cross_Hair_Left].x,
						(float)param->ptCross_hair[CrossHair::Cross_Hair_Left].y,
						(float)param->ptCross_hair[CrossHair::Cross_Hair_Right].x,
						(float)param->ptCross_hair[CrossHair::Cross_Hair_Right].y);

					double cross_dist_pixels_h
						= Math::GetDistance2D(
						(float)param->ptCross_hair[CrossHair::Cross_Hair_Top].x,
						(float)param->ptCross_hair[CrossHair::Cross_Hair_Top].y,
						(float)param->ptCross_hair[CrossHair::Cross_Hair_Bottom].x,
						(float)param->ptCross_hair[CrossHair::Cross_Hair_Bottom].y);

					double real_half_width = param->dWDistance / cross_dist_pixels_w
						* param->nwidth / 2.0;
					res->hfov = atan(real_half_width / param->dModule2Chart_dist) / PI * 180.0 * 2.0;

					double real_half_height = param->dHDistance / cross_dist_pixels_h
						* param->nheight / 2.0;
					res->vfov = atan(real_half_height / param->dModule2Chart_dist) / PI * 180.0 * 2.0;

					double a = sqrt(POWER(cross_dist_pixels_w) + POWER(cross_dist_pixels_h)) / 2;
					double b = sqrt(POWER(param->dWDistance) + POWER(param->dHDistance)) / 2;
					double c = sqrt((double)POWER(param->nwidth) + POWER(param->nheight)) / 2;
					double d = b * c / a;
					res->dfov = atan(d / param->dModule2Chart_dist) / PI * 180.0 * 2;
				}

				void SmoothFOV(
					FOV_PARAM *param,
					FOV_RESULT *res)
				{
					if (param->fov_smooth_data.cache_count < 
						param->fov_smooth_data.fov_cache_num) param->fov_smooth_data.cache_count++;

					memcpy(&param->fov_smooth_data.cache[param->fov_smooth_data.cache_idx], res, sizeof(FOV_RESULT));
					param->fov_smooth_data.cache_idx++;
					param->fov_smooth_data.cache_idx = param->fov_smooth_data.cache_idx % param->fov_smooth_data.fov_cache_num;

					double hfov = 0.0, vfov = 0.0, dfov = 0.0;
					double hfov_max = 0.0, hfov_min = 100.0, vfov_max = 0.0, vfov_min = 100.0, dfov_max = 0.0, dfov_min = 100.0;
					for (int i = 0; i < param->fov_smooth_data.cache_count; i++) {
						FOV_RESULT *res = &param->fov_smooth_data.cache[i];
						hfov += res->hfov;
						vfov += res->vfov;
						dfov += res->dfov;

						hfov_max = max(hfov_max, res->hfov);
						hfov_min = min(hfov_min, res->hfov);
						vfov_max = max(vfov_max, res->vfov);
						vfov_min = min(vfov_max, res->vfov);
						dfov_max = max(dfov_max, res->dfov);
						dfov_min = min(dfov_max, res->dfov);
					}
					int dropCacheCnt = 0;
					if (param->fov_smooth_data.cache_count == param->fov_smooth_data.fov_cache_num) {
						hfov -= hfov_max + hfov_min;
						vfov -= vfov_max + vfov_min;
						dfov -= dfov_max + dfov_min;
						dropCacheCnt = 2;
					}

					int iSmoothStackNum = param->fov_smooth_data.fov_cache_num - dropCacheCnt;
					hfov /= iSmoothStackNum;
					vfov /= iSmoothStackNum;
					dfov /= iSmoothStackNum;

					res->hfov = hfov;
					res->vfov = vfov;
					res->dfov = dfov;
				}
            }
        }
    }
}
