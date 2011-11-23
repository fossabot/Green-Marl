
#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"

//--------------------------------------------------------------
// A Back-End for GPS generation
//--------------------------------------------------------------
void gm_gps_gen::setTargetDir(const char* d)
{
    printf("%s = \n", d);
    if (dname!= NULL)
        printf("%s = \n", dname);
    assert(d!=NULL);
    if (dname != NULL)
        delete [] dname;
    dname = new char[strlen(d)+1];
    strcpy(dname, d);
}

void gm_gps_gen::setFileName(const char* f)
{
    assert(f!=NULL);
    if (fname != NULL)
        delete [] fname;
    fname = new char[strlen(f)+1];
    strcpy(fname, f);
}

bool gm_gps_gen::open_output_files()
{
    char temp[1024];
    assert(dname!=NULL);
    assert(fname!=NULL);

    sprintf(temp, "%s/%s.java", dname, fname);
    f_body = fopen(temp, "w");
    if (f_body == NULL) {
        gm_backend_error(GM_ERROR_FILEWRITE_ERROR, temp);
        return false;
    }
    Body.set_output_file(f_body);

    get_lib()->set_code_writer(&Body);

}
void gm_gps_gen::close_output_files()
{
    if (f_body!=NULL) {Body.flush();fclose(f_body); f_body = NULL;}
}





//----------------------------------------------------
// Main Generator
//----------------------------------------------------
bool gm_gps_gen::do_generate()
{

    if (!open_output_files())
        return false;

    assert(get_current_proc() != NULL);

    // sub-steps
    const char* NAMES[]= {
        "[Merging Information]",    // ... what was I thinking again?
        "[Generating Skeleton",     // create class header and state machine
        "[Generting Codes]"         // generate java code for vertex/master compute
    };

    bool is_okay = true;

    const int COUNT = sizeof(NAMES)/sizeof(const char*);
    for(int i=0;i<COUNT;i++) {
        gm_begin_minor_compiler_stage(i +1, NAMES[i]);
        switch(i) {
            case 0:
                is_okay = do_merge_msg_information();
                break;

            case 1:
                do_generate_skeleton();
                break;

            case 2:
                do_generate_main(); 
                end_class();
                close_output_files();
                break;

            case COUNT:
            default:
                assert(false);
        }

        gm_end_minor_compiler_stage();
        if (!is_okay) return is_okay;
    }
    return is_okay;
}


bool gm_gps_gen::do_merge_msg_information()
{
    return true;
}

void gm_gps_gen::do_generate_main()
{
    // dump_vertex_methods();
    do_generate_master_states();
    // dump_master_methods();
}


void gm_gps_gen::end_class()
{
    Body.pushln("}");
}

