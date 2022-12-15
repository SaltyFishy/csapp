
//Rotate
/* Rotate第一种做法:减少不必要的计算 */
    int i, j;
    int temp = dim - 1;
    for (i = 0; i < dim; i += 8){
    for (j = 0; j < dim; j += 8){
        dst[RIDX(temp - j,i,dim)] = src[RIDX(i,j,dim)];
    }
    }

/*  Rotate第二种做法:8分块 */
    int i, j;
    int temp = dim - 1;
    for (i = 0; i < dim; i += 8){
        for (j = 0; j < dim; j += 8){
            for (k = i;k < i + 8;k++){
                for (l = j;l < j + 8;l++){
                    dst[RIDX(temp - l,k,dim)] = src[RIDX(k,l,dim)];
                }
            }
        }
    }

/*  Rotate第三种做法:调整步长 */
    int i, j, k, l, t;
    int temp = dim - 1;
    for (i = 0; i < dim; i += 8){
        for (j = 0; j < dim; j += 8){
            for (l = j;l < j + 8;l++){
                t = (temp - l) * dim;
                for (k = i;k < i + 8;k++){
                    dst[t + k] = src[RIDX(k,l,dim)];
                }
            }
        }
    }
/*  神奇的负优化......*/
    int i, j, k, l;
    int temp = dim - 1;
    int temp2 = 1;
    int logicalLeft = 0;
    int modify = 1;
    while(temp2 < dim){
        logicalLeft++;
        temp2 *= 2;
        modify = temp2;
    }
    modify = dim - modify;
    for (i = 0; i < dim; i++){
        for (j = 0; j < dim; j++){
            dst[((temp - j) << logicalLeft) + (temp - j) * modify + i] = src[(i << logicalLeft) + i * modify + j];
        }
    }


//Sommth
/*sommth第一种优化:调整为内联函数*/
inline static int min(int a, int b) { return (a < b ? a : b); }
inline static int max(int a, int b) { return (a > b ? a : b); }
inline static void initialize_pixel_sum(pixel_sum* sum)
{
    sum->red = sum->green = sum->blue = 0;
    sum->num = 0;
    return;
}
inline static void accumulate_sum(pixel_sum* sum, pixel p)
{
    sum->red += (int)p.red;
    sum->green += (int)p.green;
    sum->blue += (int)p.blue;
    sum->num++;
    return;
}
inline static void assign_sum_to_pixel(pixel* current_pixel, pixel_sum sum)
{
    current_pixel->red = (unsigned short)(sum.red / sum.num);
    current_pixel->green = (unsigned short)(sum.green / sum.num);
    current_pixel->blue = (unsigned short)(sum.blue / sum.num);
    return;
}
inline static pixel avg(int dim, int i, int j, pixel* src)
{
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);
    for (ii = max(i - 1, 0); ii <= min(i + 1, dim - 1); ii++)
        for (jj = max(j - 1, 0); jj <= min(j + 1, dim - 1); jj++)
            accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);

    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel* src, pixel* dst)
{
    int i, j;

    for (i = 0; i < dim; i++)
        for (j = 0; j < dim; j++)
            dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
}

/* Smooth第二种方法:*/
void naive_smooth(int dim, pixel* src, pixel* dst)
{
    int i, j;
    //left up
    dst[0].red = (src[0].red + src[1].red + src[dim].red + src[dim + 1].red) / 4;
    dst[0].green = (src[0].green + src[1].green + src[dim].green + src[dim + 1].green) / 4;
    dst[0].blue = (src[0].blue + src[1].blue + src[dim].blue + src[dim + 1].blue) / 4;
    //right up
    dst[dim - 1].red = (src[dim - 1].red + src[dim - 2].red + src[2 * dim - 1].red + src[2 * dim - 2].red) / 4;
    dst[dim - 1].green = (src[dim - 1].green + src[dim - 2].green + src[2 * dim - 1].green + src[2 * dim - 2].green) / 4;
    dst[dim - 1].blue = (src[dim - 1].blue + src[dim - 2].blue + src[2 * dim - 1].blue + src[2 * dim - 2].blue) / 4;
    //left down
    i = dim - 1;
    j = dim;
    dst[i * j].red = (src[i * j].red + src[i * j + 1].red + src[i * j - dim].red + src[i * j - dim + 1].red) / 4;
    dst[i * j].green = (src[i * j].green + src[i * j + 1].green + src[i * j - dim].green + src[i * j - dim + 1].green) / 4;
    dst[i * j].blue = (src[i * j].blue + src[i * j + 1].blue + src[i * j - dim].blue + src[i * j - dim + 1].blue) / 4;
    //right down
    i = dim;
    j = dim;
    dst[i * j - 1].red = (src[i * j - 1].red + src[i * j - 2].red + src[i * j - dim - 1].red + src[i * j - dim - 2].red) / 4;
    dst[i * j - 1].green = (src[i * j - 1].green + src[i * j - 2].green + src[i * j - dim - 1].green + src[i * j - dim - 2].green) / 4;
    dst[i * j - 1].blue = (src[i * j - 1].blue + src[i * j - 2].blue + src[i * j - dim - 1].blue + src[i * j - dim - 2].blue) / 4;
    //up
    for (i = 1; i < dim - 1; i++) {
        dst[i].red = (src[i - 1].red + src[i].red + src[i + 1].red + src[i + dim - 1].red + src[i + dim].red + src[i + 1 + dim].red) / 6;
        dst[i].green = (src[i - 1].green + src[i].green + src[i + 1].green + src[i + dim - 1].green + src[i + dim].green + src[i + 1 + dim].green) / 6;
        dst[i].blue = (src[i - 1].blue + src[i].blue + src[i + 1].blue + src[i + dim - 1].blue + src[i + dim].blue + src[i + 1 + dim].blue) / 6;
    }
    //left
    for (i = 1; i < dim - 1; i++) {
        dst[RIDX(i, 0, dim)].red = (src[RIDX(i, 0, dim)].red + src[RIDX(i, -dim, dim)].red + src[RIDX(i, dim, dim)].red + src[RIDX(i, 1, dim)].red + src[RIDX(i, -dim, dim) + 1].red + src[RIDX(i, dim, dim) + 1].red) / 6;
        dst[RIDX(i, 0, dim)].green = (src[RIDX(i, 0, dim)].green + src[RIDX(i, -dim, dim)].green + src[RIDX(i, dim, dim)].green + src[RIDX(i, 1, dim)].green + src[RIDX(i, -dim, dim) + 1].green + src[RIDX(i, dim, dim) + 1].green) / 6;
        dst[RIDX(i, 0, dim)].blue = (src[RIDX(i, 0, dim)].blue + src[RIDX(i, -dim, dim)].blue + src[RIDX(i, dim, dim)].blue + src[RIDX(i, 1, dim)].blue + src[RIDX(i, -dim, dim) + 1].blue + src[RIDX(i, dim, dim) + 1].blue) / 6;
    }
    //right
    for (i = 1; i < dim - 1; i++) {
        dst[RIDX(i, dim - 1, dim)].red = (src[RIDX(i, dim - 1, dim)].red + src[RIDX(i - 1, dim - 1, dim)].red + src[RIDX(i + 1, dim - 1, dim)].red + src[RIDX(i, dim - 2, dim)].red + src[RIDX(i - 1, dim - 2, dim)].red + src[RIDX(i + 1, dim - 2, dim)].red) / 6;
        dst[RIDX(i, dim - 1, dim)].green = (src[RIDX(i, dim - 1, dim)].green + src[RIDX(i - 1, dim - 1, dim)].green + src[RIDX(i + 1, dim - 1, dim)].green + src[RIDX(i, dim - 2, dim)].green + src[RIDX(i - 1, dim - 2, dim)].green + src[RIDX(i + 1, dim - 2, dim)].green) / 6;
        dst[RIDX(i, dim - 1, dim)].blue = (src[RIDX(i, dim - 1, dim)].blue + src[RIDX(i - 1, dim - 1, dim)].blue + src[RIDX(i + 1, dim - 1, dim)].blue + src[RIDX(i, dim - 2, dim)].blue + src[RIDX(i - 1, dim - 2, dim)].blue + src[RIDX(i + 1, dim - 2, dim)].blue) / 6;
    }
    //down
    for (i = 1; i < dim - 1; i++) {
        dst[RIDX(dim - 1, i, dim)].red = (src[RIDX(dim - 1, i, dim)].red + src[RIDX(dim - 1, i - 1, dim)].red + src[RIDX(dim - 1, i + 1, dim)].red + src[RIDX(dim - 2, i, dim)].red + src[RIDX(dim - 2, i - 1, dim)].red + src[RIDX(dim - 2, i + 1, dim)].red) / 6;
        dst[RIDX(dim - 1, i, dim)].green = (src[RIDX(dim - 1, i, dim)].green + src[RIDX(dim - 1, i - 1, dim)].green + src[RIDX(dim - 1, i + 1, dim)].green + src[RIDX(dim - 2, i, dim)].green + src[RIDX(dim - 2, i - 1, dim)].green + src[RIDX(dim - 2, i + 1, dim)].green) / 6;
        dst[RIDX(dim - 1, i, dim)].blue = (src[RIDX(dim - 1, i, dim)].blue + src[RIDX(dim - 1, i - 1, dim)].blue + src[RIDX(dim - 1, i + 1, dim)].blue + src[RIDX(dim - 2, i, dim)].blue + src[RIDX(dim - 2, i - 1, dim)].blue + src[RIDX(dim - 2, i + 1, dim)].blue) / 6;
    }
    //middle
    for (i = 1; i <= dim - 2; i++) {
        for (j = 1; j <= dim - 2; j++) {
            dst[RIDX(i, j, dim)].red = (src[RIDX(i, j, dim)].red + src[RIDX(i, j - 1, dim)].red + src[RIDX(i, j + 1, dim)].red + src[RIDX(i - 1, j, dim)].red + src[RIDX(i - 1, j - 1, dim)].red + src[RIDX(i - 1, j + 1, dim)].red + src[RIDX(i + 1, j, dim)].red + src[RIDX(i + 1, j - 1, dim)].red + src[RIDX(i + 1, j + 1, dim)].red) / 9;
            dst[RIDX(i, j, dim)].green = (src[RIDX(i, j, dim)].green + src[RIDX(i, j - 1, dim)].green + src[RIDX(i, j + 1, dim)].green + src[RIDX(i - 1, j, dim)].green + src[RIDX(i - 1, j - 1, dim)].green + src[RIDX(i - 1, j + 1, dim)].green + src[RIDX(i + 1, j, dim)].green + src[RIDX(i + 1, j - 1, dim)].green + src[RIDX(i + 1, j + 1, dim)].green) / 9;
            dst[RIDX(i, j, dim)].blue = (src[RIDX(i, j, dim)].blue + src[RIDX(i, j - 1, dim)].blue + src[RIDX(i, j + 1, dim)].blue + src[RIDX(i - 1, j, dim)].blue + src[RIDX(i - 1, j - 1, dim)].blue + src[RIDX(i - 1, j + 1, dim)].blue + src[RIDX(i + 1, j, dim)].blue + src[RIDX(i + 1, j - 1, dim)].blue + src[RIDX(i + 1, j + 1, dim)].blue) / 9;
        }
    }
}

/* Smooth第三个版本 */
void naive_smooth(int dim, pixel* src, pixel* dst)
{
    int i, j, temp;
    //left up
    temp = 0;
    dst[temp].red = (src[temp].red + src[1].red + src[dim].red + src[dim + 1].red) / 4;
    dst[temp].green = (src[temp].green + src[1].green + src[dim].green + src[dim + 1].green) / 4;
    dst[temp].blue = (src[temp].blue + src[1].blue + src[dim].blue + src[dim + 1].blue) / 4;
    //right up
    temp = dim - 1;
    dst[temp].red = (src[temp].red + src[dim - 2].red + src[2 * dim - 1].red + src[2 * dim - 2].red) / 4;
    dst[temp].green = (src[temp].green + src[dim - 2].green + src[2 * dim - 1].green + src[2 * dim - 2].green) / 4;
    dst[temp].blue = (src[temp].blue + src[dim - 2].blue + src[2 * dim - 1].blue + src[2 * dim - 2].blue) / 4;
    //left down
    temp = dim * (dim - 1);
    dst[temp].red = (src[temp].red + src[temp + 1].red + src[temp - dim].red + src[temp - dim + 1].red) / 4;
    dst[temp].green = (src[temp].green + src[temp + 1].green + src[temp - dim].green + src[temp - dim + 1].green) / 4;
    dst[temp].blue = (src[temp].blue + src[temp + 1].blue + src[temp - dim].blue + src[temp - dim + 1].blue) / 4;
    //right down
    temp = dim * dim;
    dst[temp - 1].red = (src[temp - 1].red + src[temp - 2].red + src[temp - dim - 1].red + src[temp - dim - 2].red) / 4;
    dst[temp - 1].green = (src[temp - 1].green + src[temp - 2].green + src[temp - dim - 1].green + src[temp - dim - 2].green) / 4;
    dst[temp - 1].blue = (src[temp - 1].blue + src[temp - 2].blue + src[temp - dim - 1].blue + src[temp - dim - 2].blue) / 4;
    //up
    for (i = 1; i < dim - 1; i++) {
        dst[i].red = (src[i - 1].red + src[i].red + src[i + 1].red + src[i + dim - 1].red + src[i + dim].red + src[i + 1 + dim].red) / 6;
        dst[i].green = (src[i - 1].green + src[i].green + src[i + 1].green + src[i + dim - 1].green + src[i + dim].green + src[i + 1 + dim].green) / 6;
        dst[i].blue = (src[i - 1].blue + src[i].blue + src[i + 1].blue + src[i + dim - 1].blue + src[i + dim].blue + src[i + 1 + dim].blue) / 6;
    }
    //left
    for (i = 1; i < dim - 1; i++) {
        dst[RIDX(i, 0, dim)].red = (src[RIDX(i, 0, dim)].red + src[RIDX(i, -dim, dim)].red + src[RIDX(i, dim, dim)].red + src[RIDX(i, 1, dim)].red + src[RIDX(i, -dim, dim) + 1].red + src[RIDX(i, dim, dim) + 1].red) / 6;
        dst[RIDX(i, 0, dim)].green = (src[RIDX(i, 0, dim)].green + src[RIDX(i, -dim, dim)].green + src[RIDX(i, dim, dim)].green + src[RIDX(i, 1, dim)].green + src[RIDX(i, -dim, dim) + 1].green + src[RIDX(i, dim, dim) + 1].green) / 6;
        dst[RIDX(i, 0, dim)].blue = (src[RIDX(i, 0, dim)].blue + src[RIDX(i, -dim, dim)].blue + src[RIDX(i, dim, dim)].blue + src[RIDX(i, 1, dim)].blue + src[RIDX(i, -dim, dim) + 1].blue + src[RIDX(i, dim, dim) + 1].blue) / 6;
    }
    //right
    for (i = 1; i < dim - 1; i++) {
        dst[RIDX(i, dim - 1, dim)].red = (src[RIDX(i, dim - 1, dim)].red + src[RIDX(i - 1, dim - 1, dim)].red + src[RIDX(i + 1, dim - 1, dim)].red + src[RIDX(i, dim - 2, dim)].red + src[RIDX(i - 1, dim - 2, dim)].red + src[RIDX(i + 1, dim - 2, dim)].red) / 6;
        dst[RIDX(i, dim - 1, dim)].green = (src[RIDX(i, dim - 1, dim)].green + src[RIDX(i - 1, dim - 1, dim)].green + src[RIDX(i + 1, dim - 1, dim)].green + src[RIDX(i, dim - 2, dim)].green + src[RIDX(i - 1, dim - 2, dim)].green + src[RIDX(i + 1, dim - 2, dim)].green) / 6;
        dst[RIDX(i, dim - 1, dim)].blue = (src[RIDX(i, dim - 1, dim)].blue + src[RIDX(i - 1, dim - 1, dim)].blue + src[RIDX(i + 1, dim - 1, dim)].blue + src[RIDX(i, dim - 2, dim)].blue + src[RIDX(i - 1, dim - 2, dim)].blue + src[RIDX(i + 1, dim - 2, dim)].blue) / 6;
    }
    //down
    for (i = 1; i < dim - 1; i++) {
        dst[RIDX(dim - 1, i, dim)].red = (src[RIDX(dim - 1, i, dim)].red + src[RIDX(dim - 1, i - 1, dim)].red + src[RIDX(dim - 1, i + 1, dim)].red + src[RIDX(dim - 2, i, dim)].red + src[RIDX(dim - 2, i - 1, dim)].red + src[RIDX(dim - 2, i + 1, dim)].red) / 6;
        dst[RIDX(dim - 1, i, dim)].green = (src[RIDX(dim - 1, i, dim)].green + src[RIDX(dim - 1, i - 1, dim)].green + src[RIDX(dim - 1, i + 1, dim)].green + src[RIDX(dim - 2, i, dim)].green + src[RIDX(dim - 2, i - 1, dim)].green + src[RIDX(dim - 2, i + 1, dim)].green) / 6;
        dst[RIDX(dim - 1, i, dim)].blue = (src[RIDX(dim - 1, i, dim)].blue + src[RIDX(dim - 1, i - 1, dim)].blue + src[RIDX(dim - 1, i + 1, dim)].blue + src[RIDX(dim - 2, i, dim)].blue + src[RIDX(dim - 2, i - 1, dim)].blue + src[RIDX(dim - 2, i + 1, dim)].blue) / 6;
    }
    //middle
    for (i = 1; i <= dim - 2; i++) {
        for (j = 1; j <= dim - 2; j++) {
            dst[RIDX(i, j, dim)].red = (src[RIDX(i, j, dim)].red + src[RIDX(i, j - 1, dim)].red + src[RIDX(i, j + 1, dim)].red + src[RIDX(i - 1, j, dim)].red + src[RIDX(i - 1, j - 1, dim)].red + src[RIDX(i - 1, j + 1, dim)].red + src[RIDX(i + 1, j, dim)].red + src[RIDX(i + 1, j - 1, dim)].red + src[RIDX(i + 1, j + 1, dim)].red) / 9;
            dst[RIDX(i, j, dim)].green = (src[RIDX(i, j, dim)].green + src[RIDX(i, j - 1, dim)].green + src[RIDX(i, j + 1, dim)].green + src[RIDX(i - 1, j, dim)].green + src[RIDX(i - 1, j - 1, dim)].green + src[RIDX(i - 1, j + 1, dim)].green + src[RIDX(i + 1, j, dim)].green + src[RIDX(i + 1, j - 1, dim)].green + src[RIDX(i + 1, j + 1, dim)].green) / 9;
            dst[RIDX(i, j, dim)].blue = (src[RIDX(i, j, dim)].blue + src[RIDX(i, j - 1, dim)].blue + src[RIDX(i, j + 1, dim)].blue + src[RIDX(i - 1, j, dim)].blue + src[RIDX(i - 1, j - 1, dim)].blue + src[RIDX(i - 1, j + 1, dim)].blue + src[RIDX(i + 1, j, dim)].blue + src[RIDX(i + 1, j - 1, dim)].blue + src[RIDX(i + 1, j + 1, dim)].blue) / 9;
        }
    }
}