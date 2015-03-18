#include "stdafx.h"
#include "behavior_aux.h"


namespace htmlayout 
{

/*
BEHAVIOR: form
  Sends form data to the server.

  ATTENTION!: This is just a template!
  You shall implement on_data_arrived in suitable way 

TYPICAL USE CASE and SAMPLE:
  sdk/html_samples/behaviors/form-test.htm

*/

// Each form has unique form_instance object assosiated with the element. 
// Reason: each form has its own collection of initial_values (to be reset).

struct form_instance: public event_handler
{
    named_values initial_values;

    struct pair 
    {
      std::wstring name;
      std::wstring value;
    };
   
    form_instance():event_handler (HANDLE_INITIALIZATION | HANDLE_BEHAVIOR_EVENT | HANDLE_DATA_ARRIVED) 
    {
    }

    virtual void attached  (HELEMENT he ) 
    { 
      dom::element the_form = he;
      get_values(the_form,initial_values);
    } 

    virtual void detached  (HELEMENT he ) 
    { 
      delete this; // don't need it anymore.
    } 

    virtual BOOL on_event (HELEMENT he, HELEMENT target, BEHAVIOR_EVENTS type, UINT reason ) 
    { 
      if( type != BUTTON_CLICK )
        return FALSE;

      // handling only BUTTON CLICKs here

      dom::element btn = target;  // button - initiator
      const wchar_t* btn_type =  btn.get_attribute("type");

      dom::element the_form = he; // this is our form element we attached to
    
      if( wcseqi( btn_type , L"reset" ) )
      {
        set_values(the_form, initial_values);
        return TRUE; // handled - don't bubble it further
      }
      else if( wcseqi( btn_type , L"submit" ) )
        ;
      else
        return FALSE; // neither reset nor submit

    
      const wchar_t* action =  the_form.get_attribute("action");
      if( !action )
      {
        assert(false); // form without action and we've got submit.
        return FALSE; 
      }

      std::wstring uri = action;

      bool do_post = wcseqi( the_form.get_attribute("method"), L"post");

      named_values values;

      if(!get_values(the_form,values))
      {
        assert(false); // hey, where are my inputs?
        return FALSE;
      }

      // finally, we've got method, action and values to send.


      std::vector<pair> pairs;
     
      for( named_values_iterator it = values.begin(); it != values.end(); it++ )
      {
        pair p; 
        p.name = (*it).first;
        p.value = (*it).second.to_string();
        pairs.push_back(p);
      }

      if( pairs.size() > 1000)
      {
        assert(false); // more than 1000 fields, are you crazy?
        //return FALSE;
      }

      REQUEST_PARAM *rq_params = (REQUEST_PARAM *)alloca(sizeof(REQUEST_PARAM) * pairs.size());
      for(int n = pairs.size() - 1; n >= 0 ; --n)
      {
        rq_params[n].name = pairs[n].name.c_str();
        rq_params[n].value = pairs[n].value.c_str();
      }

      HLDOM_RESULT hr = ::HTMLayoutHttpRequest( 
          the_form,                         // element to deliver data 
          uri.c_str(),                      // url 
          HLRT_DATA_HTML,                   // data type, see HTMLayoutResourceType.
          do_post? POST_ASYNC: GET_ASYNC,   // one of REQUEST_TYPE values
          rq_params,                        // parameters 
          pairs.size()                      // number of parameters 
          );

      assert( hr == HLDOM_OK );

      return TRUE;
    }

    virtual BOOL on_data_arrived (HELEMENT he, HELEMENT initiator, LPCBYTE data, UINT dataSize, UINT dataType ) 
    { 
      if(dataType != HLRT_DATA_HTML)
        return TRUE; 

      dom::element the_form = he; // this is our form element we attached to
      htmlayout::dialog dlg(the_form.get_element_hwnd(true));
      dlg.show(data,dataSize);

      return TRUE;

    }
  
};



struct form: public behavior
{
    form(): behavior(HANDLE_BEHAVIOR_EVENT, "form") {}

    // this behavior has unique instance for each element it is attached to
    virtual event_handler* attach (HELEMENT /*he*/ ) 
    { 
      return new form_instance(); 
    }
};

// instantiating and attaching it to the global list
form form_instance;
// force it to be included
#pragma comment(linker, FORCE_LINKAGE_OF(form))


} // htmlayout namespace

