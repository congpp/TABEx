#include "gaussianBlur.h"
#include <stdlib.h>
#include <math.h>

namespace ImageFilter
{

    CGaussBlurFilter::CGaussBlurFilter()
        : m_r(-1)
        , m_sigma(-1.0)
        , m_pTempl(nullptr)
        , m_dwCompositionClr(0)
    {
    }

    CGaussBlurFilter::~CGaussBlurFilter()
    {
        Reset();
    }

    void CGaussBlurFilter::Reset()
    {
        m_r = -1;
        m_sigma = -1.0;
        if (m_pTempl != nullptr)
        {
            free(m_pTempl);
            m_pTempl = nullptr;
        }
    }

    bool CGaussBlurFilter::Filter(void* pSrc, void* pDest, int width, int height, int bpp)
    {
        if (pSrc == nullptr || pDest == nullptr || pSrc == pDest)
            return false;

        //只处理32 bpp
        if (bpp != 32)
            return false;

        if (m_r < 0 || m_pTempl == nullptr)
            return false;

        int absHeight = (height >= 0) ? height : (-height);
        int stride = (width * bpp + 31) / 32 * 4;
        int pixelSize = bpp / 8;

        //申请缓冲区，存储中间结果
        void* pTemp = malloc(size_t(stride * absHeight));
        if (pTemp == nullptr)
            return false;

        //单线程
        GBThreadParam params;

        params.pSrc  = static_cast<uchar*>(pSrc);
        params.pDest = static_cast<uchar*>(pTemp);
        params.width = width;
        params.height = absHeight;
        params.stride = stride;
        params.pixelSize = pixelSize;
        params.r = m_r;
        params.pTempl = m_pTempl;
        params.rowBegin = 0;
        params.rowEnd = absHeight;
        params.bHorz = true;
        params.channel = FC_ALL;
        params.compositeColor = m_dwCompositionClr;

        GaussBlurThreadProc32(&params);

        params.pSrc = static_cast<uchar*>(pTemp);
        params.pDest = static_cast<uchar*>(pDest);
        params.bHorz = false;

        GaussBlurThreadProc32(&params);

        free(pTemp);
        return true;
    }

    void CGaussBlurFilter::SetSigma(double sigma)
    {
        int i;
        m_sigma = sigma;
        m_r = static_cast<int>(m_sigma * 3 + 0.5);
        if (m_r <= 0) m_r = 1;

        //分配模板
        if (m_pTempl)
            free(m_pTempl);

        m_pTempl = nullptr;
        m_pTempl = reinterpret_cast<double*>(malloc(sizeof(double) * static_cast<unsigned>((m_r + 1))));

        //分配失败？
        if (m_pTempl == nullptr)
            return;

        //计算 p[0] 灰度值为1 的模板面
        double k1 = (-0.5) / (m_sigma * m_sigma);
        for (i = 0; i <= m_r; i++)
            m_pTempl[i] = exp(k1 * i * i);

        //计算模板加权总和
        double sum = m_pTempl[0];
        for (i = 1; i <= m_r; i++)
        {
            sum += (m_pTempl[i] * 2);
        }

        //归一化
        sum = (1.0 / sum); //取倒数
        for (i = 0; i <= m_r; i++)
            m_pTempl[i] *= sum;
    }

    void CGaussBlurFilter::SetRadius(int r)
    {
        double sigma = (r * 1.0 - 0.5) / 3.0;
        SetSigma(sigma);
    }

    void ImageFilter::CGaussBlurFilter::SetCompositionColor(unsigned dwClr /*= 0*/)
    {
        m_dwCompositionClr = dwClr;
    }

    //线程的入口点: 处理 32 bpp 图像
    unsigned CGaussBlurFilter::GaussBlurThreadProc32(void* lpParameters)
    {
        GBThreadParam *pInfo = static_cast<GBThreadParam*>(lpParameters);

        double result[4];
        int row, col, subRow, subCol, MaxVal, x, x1;
        int* pSubVal, *pRefVal;

        if (pInfo->bHorz)
        {
            //水平方向
            pSubVal = &subCol;
            pRefVal = &col;
            MaxVal = pInfo->width - 1;
        }
        else
        {
            //垂直方向
            pSubVal = &subRow;
            pRefVal = &row;
            MaxVal = pInfo->height - 1;
        }

        uchar* pSrcPixel = nullptr;
        uchar* pDestPixel = nullptr;

        for (row = pInfo->rowBegin; row < pInfo->rowEnd; ++row)
        {
            for (col = 0; col < pInfo->width; ++col)
            {
                pDestPixel = pInfo->pDest + pInfo->stride * row + pInfo->pixelSize * col;

                result[0] = 0;
                result[1] = 0;
                result[2] = 0;
                result[3] = 0;

                subRow = row;
                subCol = col;

                for (x = -pInfo->r; x <= pInfo->r; x++)
                {
                    x1 = (x >= 0) ? x : (-x);
                    *pSubVal = *pRefVal + x;

                    //边界处理：Photoshop 采用的是方法1。
                    //方法1：取边缘像素（图像边缘像素向内部扩散！）
                    if (*pSubVal < 0) *pSubVal = 0;
                    else if (*pSubVal > MaxVal) *pSubVal = MaxVal;

                    //方法2：取当前像素（使得越靠近图像边缘的地方越清晰）
                    /*
                    if(*pSubVal < 0 || *pSubVal > MaxVal)
                    *pSubVal = *pRefVal;
                    */

                    pSrcPixel = pInfo->pSrc + pInfo->stride * subRow + pInfo->pixelSize * subCol;

                    result[0] += pSrcPixel[0] * pInfo->pTempl[x1];
                    result[1] += pSrcPixel[1] * pInfo->pTempl[x1];
                    result[2] += pSrcPixel[2] * pInfo->pTempl[x1];
                    result[3] += pSrcPixel[3] * pInfo->pTempl[x1];

                }

                pDestPixel[0] = uchar(result[0]);
                pDestPixel[1] = uchar(result[1]);
                pDestPixel[2] = uchar(result[2]);
                pDestPixel[3] = uchar(result[3]);

            }
        }
        return 0;
    }

}
