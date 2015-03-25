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

    #if CV_ENABLE_UNROLLED
    for(; i <= m - 4; i += 4 )
    {
        uchar* d0 = (uchar*)(dst + dstep*i);
        uchar* d1 = (uchar*)(dst + dstep*(i+1));
        uchar* d2 = (uchar*)(dst + dstep*(i+2));
        uchar* d3 = (uchar*)(dst + dstep*(i+3));

        for( j = 0; j <= n - 4; j += 4 )
        {
            const uchar* s0 = (const uchar*)(src + i*sizeof(uchar) + sstep*j);
            const uchar* s1 = (const uchar*)(src + i*sizeof(uchar) + sstep*(j+1));
            const uchar* s2 = (const uchar*)(src + i*sizeof(uchar) + sstep*(j+2));
            const uchar* s3 = (const uchar*)(src + i*sizeof(uchar) + sstep*(j+3));

            d0[j] = s0[0]; d0[j+1] = s1[0]; d0[j+2] = s2[0]; d0[j+3] = s3[0];
            d1[j] = s0[1]; d1[j+1] = s1[1]; d1[j+2] = s2[1]; d1[j+3] = s3[1];
            d2[j] = s0[2]; d2[j+1] = s1[2]; d2[j+2] = s2[2]; d2[j+3] = s3[2];
            d3[j] = s0[3]; d3[j+1] = s1[3]; d3[j+2] = s2[3]; d3[j+3] = s3[3];
        }

        for( ; j < n; j++ )
        {
            const uchar* s0 = (const uchar*)(src + i*sizeof(uchar) + j*sstep);
            d0[j] = s0[0]; d1[j] = s0[1]; d2[j] = s0[2]; d3[j] = s0[3];
        }
    }
    #endif
    for( ; i < m; i++ )
    {
        uchar* d0 = (uchar*)(dst + dstep*i);
        j = 0;
        #if CV_ENABLE_UNROLLED
        for(; j <= n - 4; j += 4 )
        {
            const uchar* s0 = (const uchar*)(src + i*sizeof(uchar) + sstep*j);
            const uchar* s1 = (const uchar*)(src + i*sizeof(uchar) + sstep*(j+1));
            const uchar* s2 = (const uchar*)(src + i*sizeof(uchar) + sstep*(j+2));
            const uchar* s3 = (const uchar*)(src + i*sizeof(uchar) + sstep*(j+3));

            d0[j] = s0[0]; d0[j+1] = s1[0]; d0[j+2] = s2[0]; d0[j+3] = s3[0];
        }
        #endif
        for( ; j < n; j++ )
        {
            const uchar* s0 = (const uchar*)(src + i*sizeof(uchar) + j*sstep);
            d0[j] = s0[0];
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
