
#define MAX_OPS 25

static struct {
    VALUE proc;
    int assigned, resize;
} op_table[MAX_OPS];

static void op_0(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[0].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[0].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_1(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[1].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[1].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_2(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[2].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[2].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_3(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[3].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[3].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_4(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[4].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[4].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_5(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[5].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[5].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_6(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[6].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[6].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_7(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[7].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[7].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_8(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[8].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[8].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_9(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[9].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[9].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_10(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[10].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[10].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_11(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[11].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[11].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_12(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[12].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[12].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_13(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[13].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[13].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_14(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[14].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[14].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_15(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[15].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[15].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_16(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[16].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[16].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_17(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[17].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[17].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_18(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[18].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[18].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_19(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[19].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[19].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_20(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[20].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[20].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_21(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[21].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[21].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_22(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[22].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[22].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_23(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[23].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[23].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static void op_24(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
{
    int rv, i, size, length;
    char *data;
    VALUE obj;

    rv = MPI_Type_size(*dtype, &size);
    mpi_exception(rv);

    for (i = 0; i < *len; i++) {
        VALUE str, arg1, arg2;

        str = rb_str_new(((char *)invec) + (i * size), size);
        arg1 = rb_funcall(mMarshal, id_load, 1, str);

        str = rb_str_new(((char *)inoutvec) + (i * size), size);
        arg2 = rb_funcall(mMarshal, id_load, 1, str);

        obj = rb_funcall(op_table[24].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[24].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

static MPI_User_function *fn_table[MAX_OPS] = {
    op_0,
    op_1,
    op_2,
    op_3,
    op_4,
    op_5,
    op_6,
    op_7,
    op_8,
    op_9,
    op_10,
    op_11,
    op_12,
    op_13,
    op_14,
    op_15,
    op_16,
    op_17,
    op_18,
    op_19,
    op_20,
    op_21,
    op_22,
    op_23,
    op_24,
};
