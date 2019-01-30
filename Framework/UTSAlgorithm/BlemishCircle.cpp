#include "StdAfx.h"
#include "Algorithm.h"
#include "libCircleBlemish.h"

namespace UTS
{
    namespace Algorithm
    {
        namespace Blemish
        {
            namespace Blemish_Circle
            {
                void CircleBlemish_RGB(
                    unsigned char *pBmpBuffer,
                    int nWidth,
                    int nHeight,
                    const BlemishCircleParam &param,
                    vector<Circle> &vResult)
                {
                    assert(nullptr != pBmpBuffer);
                    assert(nWidth > 0);
                    assert(nHeight > 0);
                    assert(param.nMaxRadius > 0);
                    assert(param.nMinRadius > 0);
                    assert(param.dThreshold >= 0);
                    assert(param.nMaxRadius >= param.nMinRadius);

                    unsigned char *pYbuffer = new unsigned char[nWidth * nHeight];
                    Image::CImageProc::GetInstance().Cal_RGBtoYBuffer(pBmpBuffer, nWidth, nHeight, pYbuffer);

                    CircleBlemish_Y(pYbuffer, nWidth, nHeight, param, vResult);

                    RELEASE_ARRAY(pYbuffer);
                }

                void CircleBlemish_Y(
                    unsigned char *pYBuffer,
                    int nWidth,
                    int nHeight,
                    const BlemishCircleParam &param,
                    std::vector<Circle> &vResult)
                {
                    assert(nullptr != pYBuffer);
                    assert(nWidth > 0);
                    assert(nHeight > 0);
                    assert(param.nMaxRadius > 0);
                    assert(param.nMinRadius > 0);
                    assert(param.dThreshold >= 0);
                    assert(param.nMaxRadius >= param.nMinRadius);

                    vResult.clear();

                    GreatXu::BlemishCircleParam gxParam;
                    vector<GreatXu::Circle> gxResult;
                    memcpy(&gxParam, &param, sizeof(param));
                    GreatXu::DoCircleBlemish(pYBuffer, nWidth, nHeight, gxParam, gxResult);
                    for (size_t i = 0; i < gxResult.size(); i++)
                    {
                        Circle circle;
                        circle.r = gxResult[i].r;
                        circle.x = gxResult[i].x;
                        circle.y = gxResult[i].y;
                        vResult.push_back(circle);
                    }
                }
            }
        }
    }
}
