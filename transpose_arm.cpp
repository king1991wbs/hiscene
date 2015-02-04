void cv::transpose( InputArray _src, OutputArray _dst )
{
    Mat src = _src.getMat();
    size_t esz = src.elemSize();
    CV_Assert( src.dims <= 2 && esz <= (size_t)32 );

    _dst.create(src.cols, src.rows, src.type());
    Mat dst = _dst.getMat();

    // handle the case of single-column/single-row matrices, stored in STL vectors.
    if( src.rows != dst.cols || src.cols != dst.rows )
    {
        CV_Assert( src.size() == dst.size() && (src.cols == 1 || src.rows == 1) );
        src.copyTo(dst);
        return;
    }

    if( dst.data == src.data )
    {
        transposeI_8u( dst.data, dst.step, dst.rows );
    }
    else
    {
        transpose_8u( src.data, src.step, dst.data, dst.step, src.size() );
    }
}


void
transpose_8u( const uchar* src, size_t sstep, uchar* dst, size_t dstep, Size sz )
{
    int i=0, j, m = sz.width, n = sz.height;

    for( ; i < n; ++i)
    {
        const uint8_t* s0 = (const uint8_t*)(src + i*sstep);
        uint8x8_t sData;

        j = 0;
        uint8_t* d0 = (uint8_t*)(dst + i*sizeof(uchar) + j*dstep);

        for(int k = 0; k < (sstep/sizeof(uchar)) / 8; ++k)
        {
            sData = vld1_u8(s0);
            for(int q = 0; q < 8; ++q)
            {
                *d0 = vget_lane_u8(sData, q);
                ++j;
                d0 = (uint8_t*)(dst + i*sizeof(uchar) + j*dstep);
            }
            s0 += 8;
        }

        for( ; j < m; ++j)
        {
            *d0 = *s0;
            d0 = (uint8_t*)(dst + i*sizeof(uchar) + j*dstep);
            ++s0;
        }
    }
}

void
transposeI_8u( uchar* data, size_t step, int n )
{
    int i, j;
    for( i = 0; i < n; i++ )
    {
        uchar* row = (uchar*)(data + step*i);
        uchar* data1 = data + i*sizeof(uchar);
        for( j = i+1; j < n; j++ )
            std::swap( row[j], *(uchar*)(data1 + step*j) );
    }
}
