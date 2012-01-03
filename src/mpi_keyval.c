#include "mpi.h"
#include "ruby/ruby.h"
#include "global.h"

VALUE cKeyval;

/* The user needn't know that this is an int */
struct mpi_keyval {
	int keyval;
};

static int keyval_copy(MPI_Comm oldcomm, int keyval, void *data, 
		                    void *attr_in, void *attr_out, int *flag)
{
	struct mpi_attr_val *in, *out;

	in = attr_in;
	out = ALLOC(struct mpi_attr_val);
	out->data = ALLOC_N(char, in->len + 1);

	MEMCPY(out->data, in->data, char, in->len + 1);

	*(struct mpi_attr_val **)attr_out = out;

	*flag = 1;

	return 0;
}

static int keyval_copy_uniq(MPI_Comm oldcomm, int keyval, void *data, 
		                    void *attr_in, void *attr_out, int *flag)
{
	struct mpi_attr_val *in, *out;

	in = attr_in;
	out = ALLOC(struct mpi_attr_val);
	out->data = ALLOC_N(char, in->len + 1);

	MEMCPY(out->data, in->data, char, in->len + 1);

	*(struct mpi_attr_val **)attr_out = out;

	*flag = 0;

	return 0;
}

static int keyval_delete(MPI_Comm oldcomm, int keyval, void *attr, void *data)
{
	struct mpi_attr_val *in;

	free(in->data);
	free(in);

	return 0;
}

int keyval_get_keyval(VALUE rkv)
{
    struct mpi_keyval *kv;
    
    Data_Get_Struct(rkv, struct mpi_keyval, kv);

    return kv->keyval;
}

static void keyval_free(struct mpi_keyval *kv)
{
	int rv;

	rv = MPI_Keyval_free(&kv->keyval);
	free(kv);
}

static VALUE keyval_create(VALUE self, VALUE uniq)
{
	int rv;
    VALUE tdata;
	struct mpi_keyval *kv;

	kv = ALLOC(struct mpi_keyval);
	if ((uniq != Qfalse) && (uniq != Qnil)) {
		rv = MPI_Keyval_create(keyval_copy_uniq, keyval_delete, &kv->keyval, 
				               NULL);
		mpi_exception(rv);
	} else {
		rv = MPI_Keyval_create(keyval_copy, keyval_delete, &kv->keyval, NULL);
		mpi_exception(rv);
	}

    tdata = Data_Wrap_Struct(cKeyval, NULL, keyval_free, kv);
    rb_obj_call_init(tdata, 0, NULL);
	
	return tdata;
}

void Init_Keyval()
{
    cKeyval = rb_define_class_under(mMPI, "Keyval", rb_cObject);

	rb_define_singleton_method(cKeyval, "create", keyval_create, 1);
}
