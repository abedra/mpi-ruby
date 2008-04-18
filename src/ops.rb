#!/usr/bin/env ruby

# Horrible hack to get "dynamic" operations

if ARGV.length > 0 then
	size = ARGV[0].to_i
else
	size = 25
end

puts "
#define MAX_OPS #{size}

static struct {
    VALUE proc;
    int assigned, resize;
} op_table[MAX_OPS];

"

size.times do |i|
	puts "static void op_#{i}(void *invec, void *inoutvec, int *len, MPI_Datatype *dtype)
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

        obj = rb_funcall(op_table[#{i}].proc, id_call, 2, arg1, arg2);

        str = rb_funcall(mMarshal, id_dump, 1, obj);
        data = rb_str2cstr(str, &length);

        if (length + 1 > size) {
            op_table[#{i}].resize = length + 1;
        } else {
            MEMCPY(((char *)inoutvec) + (i * size), data, char, length + 1);
        }
    }
}

"
end

puts "static MPI_User_function *fn_table[MAX_OPS] = {"
size.times do |i|
	puts "    op_#{i},"
end
puts "};"
