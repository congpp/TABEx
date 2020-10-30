#pragma once
namespace ImageFilter
{
    enum FilterParamType
    {
        //////////////////////////////////////////////////////////////////////////
        FPT_COMMON = 0x0000,
        //像素格式，一些需要区分通道的算法需要用到, see enum FilterPixelFormat: ALL Filter
        //LPVOID => INT*
        FPT_PIXEL_FORMAT,
        //设置滤镜操作的channel，see enum FilterChannel：ALL Filter
        //LPVOID => INT*
        FPT_FILTER_CHANNEL,
        //调色板
        //LPVOID => FilterPalette*
        FPT_FILTER_PALATE,

        //////////////////////////////////////////////////////////////////////////
        FPT_GAUSIAN_BEGIN = 0x1000,
        //高斯半径（方差的平方根）
        //LPVOID => DOUBLE*
        FPT_GAUSIAN_SIGMA,
        //高斯半径（与SIGMA二选其一设置即可，自动计算互转，不过SIGMA的精度会高很多）
        //LPVOID => INT*
        FPT_GAUSIAN_RADIUS,
        //设置算法的混合颜色：Gaussian
        //LPVOID => DWORD*
        FPT_GAUSIAN_COMPOSITION_COLOR,
        //高对比保留的灰度[1,255]，默认127
        //LPVOID => INT*
        FPT_HIGH_PASS_GRAYSCALE = 0x1100,
    };

    enum FilterChannel
    {
        FC_A = 0x08,
        FC_R = 0x04,
        FC_G = 0x02,
        FC_B = 0x01,
        FC_ALL = 0x0F,
    };

    typedef unsigned char uchar;

    //
    // 传递到线程的参数
    struct GBThreadParam
    {
        int r;
        double* pTempl;
        uchar* pSrc;	//Src  位图的位图数据起点 （对所有线程，pSrc，pDest 是相同的）
        uchar* pDest;	//Dest 位图的位图数据起点

        int width;		//图像宽度
        int height;		//图像高度（已经被取绝对值）

        //处理的行范围：[rowBegin, rowEnd) ，不包括 rowEnd （每个线程不同）
        int rowBegin;
        int rowEnd;
        int stride;		//扫描行宽度（bytes）
        int pixelSize;	//像素大小 =bpp/8;

        bool bHorz;		//true-水平模糊，false-纵向模糊

        FilterChannel channel;  //channel
        unsigned compositeColor;  //GBM_ALPHA_ONLY时的颜色, RGB。32bpp才有效
    };

    //
    //高斯模糊算法
    //高斯模糊算法的基本思路是：
    //把每一个像素的值设置为：以当前像素为中心，半径为 sigma 的矩形内所有像素点值乘以权重，再累加的和
    //这个权重的值，来自三维高斯函数，所以他叫高斯模糊
    class CGaussBlurFilter
    {
    public:
        CGaussBlurFilter();
        virtual ~CGaussBlurFilter();

        //设置参数（此时会分配模板）
        void SetSigma(double sigma);
        void SetRadius(int r);
        //
        void SetCompositionColor(unsigned dwClr = 0);

        //释放模板
        void Reset();

        //处理图像
        // @param [in] lpSrcBits 像素数据的起始地址，必须以 4 bytes 对齐，
        // @param [out] lpDestBits 输出像素，大小需跟lpScrBits一样大
        // 注意：不论高度为正或者负，lpBits 都必须为所有像素中地址值最低的那个像素的地址。
        // @param bmWidth, bmHeight: 图像宽度和高度（像素），高度允许为负值；
        // @param bpp: 位深度，支持 8（灰度）, 24（真彩色）， 32
        bool Filter(void* pSrc, void* pDest, int width, int height, int bpp);

    protected:

        //
        // Thread Function Declares;
        //
        unsigned GaussBlurThreadProc8(void* lpParameters);
        unsigned GaussBlurThreadProc24(void* lpParameters);
        unsigned GaussBlurThreadProc32(void* lpParameters);


    protected:
        int     m_r;		        //完整模板正方形的边长为 (2 * r + 1)
        double  m_sigma;		    //高斯半径（方差的平方根）
        double* m_pTempl;	        //模板T[r+1];
        unsigned   m_dwCompositionClr;
    };

    
}
