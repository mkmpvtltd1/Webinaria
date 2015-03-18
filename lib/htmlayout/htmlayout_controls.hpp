/*
 * Terra Informatica Lightweight Embeddable HTMLayout control
 * http://terrainformatica.com/htmlayout
 * 
 * Active DOM elements (a.k.a windowless controls)
 * 
 * The code and information provided "as-is" without
 * warranty of any kind, either expressed or implied.
 * 
 * 
 * (C) 2003-2005, Andrew Fedoniouk (andrew@terrainformatica.com)
 */

#ifndef __htmlayout_controls_hpp__
#define __htmlayout_controls_hpp__

#pragma once

/*!\file
\brief Controls and their values.
*/

#include <stdio.h> // snprintf

#include "htmlayout_dom.hpp"
#include "htmlayout_value.hpp"
#include "htmlayout_aux.h" // utf8 

namespace htmlayout 
{

/** Control types.
 *  Control here is any dom element having appropriate behavior applied
 **/
  enum CTL_TYPE
  {
    CTL_UNKNOWN = 0,      ///< This dom element does not appear to be a control.
    CTL_EDIT,             ///< Single line edit box.
    CTL_NUMERIC,          ///< Numeric input with optional spin buttons.
    CTL_BUTTON,           ///< Command button.
    CTL_CHECKBOX,         ///< CheckBox (button).
    CTL_RADIO,            ///< OptionBox (button).
    CTL_SELECT_SINGLE,    ///< Single select, ListBox or TreeView.
    CTL_SELECT_MULTIPLE,  ///< Multiselectable select, ListBox or TreeView.
    CTL_DD_SELECT,        ///< Dropdown single select.
    CTL_TEXTAREA,         ///< Multiline TextBox.
    CTL_HTMLAREA,         ///< WYSIWYG HTML editor.
    CTL_PASSWORD,         ///< Password input element.
    CTL_PROGRESS,         ///< Progress element.
    CTL_SLIDER,           ///< Slider input element.  
    CTL_DECIMAL,          ///< Decimal number input element.
    CTL_CURRENCY,         ///< Currency input element.
  };

/** Get type of input control this DOM element behaves as. 
 * \param[in] el const dom::element&, The element.
 **/
  inline CTL_TYPE get_ctl_type(const dom::element& el)
  {
    const wchar_t* bn = el.get_style_attribute("behavior");
    if( bn == 0 || bn[0] == 0 )
      return CTL_UNKNOWN;

    if( wcscmp(bn, L"edit") == 0 ) return CTL_EDIT;
    if( wcscmp(bn, L"password") == 0 ) return CTL_PASSWORD;
    if( wcscmp(bn, L"number") == 0 ) return CTL_NUMERIC;
    if( wcscmp(bn, L"radio" ) == 0 ) return CTL_RADIO;
    if( wcscmp(bn, L"check") == 0 ) return CTL_CHECKBOX;
    if( wcscmp(bn, L"button") == 0 ) return CTL_BUTTON;
    if( wcscmp(bn, L"decimal") == 0 ) return CTL_DECIMAL;
    if( wcscmp(bn, L"currnecy") == 0 ) return CTL_CURRENCY;
    if( strcmp(el.get_element_type(), "button") == 0 ) return CTL_BUTTON;
    if( wcscmp(bn, L"select") == 0 ) 
    {
      if( el.get_attribute("multiple") )
        return CTL_SELECT_MULTIPLE;
      else
        return CTL_SELECT_SINGLE;   
    }
    if( wcscmp(bn, L"dropdown-select") == 0 ) return CTL_DD_SELECT;
    if( wcscmp(bn, L"textarea") == 0 ) return CTL_TEXTAREA;
    if( wcscmp(bn, L"htmlarea") == 0 ) return CTL_HTMLAREA;
    if( wcscmp(bn, L"progress") == 0 ) return CTL_PROGRESS;
    if( wcscmp(bn, L"hslider") == 0 || wcscmp(bn, L"vslider")) return CTL_SLIDER;
    return CTL_UNKNOWN;
  }


  struct  selected_cb: dom::callback 
  {
    std::vector<dom::element> elements;
    inline bool on_element(HELEMENT he) 
    { 
      elements.push_back(he);
      return false; // keep enumerating
    }
  };

  inline value_t get_radio_index( dom::element& el )
  {
    selected_cb selected;
    dom::element r = el.parent(); // ATTN: I assume here that all radios in the group belong to the same parent!
    r.find_all(&selected, "[type='radio'][name='%S']", el.get_attribute("name"));
    for( unsigned int n = 0; n < selected.elements.size(); ++n )
      if ( selected.elements[n].get_state(STATE_CHECKED) )
        return value_t(int(n)); 
    return value_t();
  }

  inline void set_radio_index( dom::element& el, const value_t& t  )
  {
    selected_cb selected;
    dom::element r = el.parent(); // ATTN: I assume here that all radios in the group belong to the same parent!
    r.find_all(&selected, "[type='radio'][name='%S']", el.get_attribute("name"));
    unsigned int idx = (unsigned int)t.get(0);
    for( unsigned int n = 0; n < selected.elements.size(); ++n )
    {
      dom::element& e = selected.elements[n];
      if ( n == idx)
          e.set_state(STATE_CHECKED, 0);
      else
          e.set_state(0, STATE_CHECKED);
    }
  }


  // returns bit mask - checkboxes set
  inline value_t get_checkbox_bits(dom::element& el )
  {
    selected_cb selected;
    dom::element r = el.parent(); // ATTN: I assume here that all checkboxes in the group belong to the same parent!
    r.find_all(&selected, "[type='checkbox'][name='%S']", el.get_attribute("name"));
    int m = 1, v = 0;
    for( unsigned int n = 0; n < selected.elements.size(); ++n, m <<= 1 )
      if ( selected.elements[n].get_state(STATE_CHECKED) ) v |= m;
    return selected.elements.size()==1?value_t(v==1):value_t(v); // for alone checkbox we return true/false 
  }

  // sets checkboxes by bit mask 
  inline void set_checkbox_bits(dom::element& el, const value_t& t )
  {
    selected_cb selected;
    dom::element r = el.parent(); // ATTN: I assume here that all checkboxes in the group belong to the same parent!
    r.find_all(&selected, "[type='checkbox'][name='%S']", el.get_attribute("name"));
    int m = 1, v = selected.elements.size()==1?(t.get(false)?1:0):t.get(0);
    for( unsigned int n = 0; n < selected.elements.size(); ++n, m <<= 1 )
    {
      dom::element& e = selected.elements[n];
      if( (v & m) != 0)
          e.set_state(  STATE_CHECKED, 0 ) ;
      else
          e.set_state(  0, STATE_CHECKED ) ;
    }
  }
  
  inline value_t get_option_value(const dom::element& opt )
  {
    const wchar_t* val = opt.get_attribute("value");
    if( val ) return value_t::from_string(val);
    return value_t(opt.get_text());
  }

  // single select
  inline value_t get_select_value(dom::element& el )
  {
    value_t v;
    dom::element opt = el.find_first("option:checked,[role='option']:checked"); // select all selected <option>s
    if( opt.is_valid() )
      v = get_option_value(opt);
    return v;
  }

  // single select
  inline void set_select_value(dom::element& el, const value_t& t )
  {
    value_t v;
    std::wstring ws = t.to_string();
    dom::element new_opt = el.find_first("option[value='%S'],[role='option'][value='%S']", ws.c_str(), ws.c_str()); // find it
    if( new_opt.is_valid() )
      new_opt.set_state( STATE_CHECKED | STATE_CURRENT, 0 ); // set state
  }

  // multi-select - returns array of selected values
  inline value_t get_select_values(dom::element& el )
  {
    selected_cb selected;
    el.find_all(&selected, "option:checked,[role='option']:checked"); // select all selected <option>s

    if(selected.elements.size() == 0) 
      return value_t();

    std::vector<value_t> values(selected.elements.size());

    for( unsigned int n = 0; n < selected.elements.size(); ++n )
      values[n] = get_option_value(selected.elements[n]);

    return value_t(&values[0], values.size()); 
  }

  // multi-select - select values
  inline void set_select_values(dom::element& el, const value_t& val_array )
  {
    
    selected_cb selected;
    el.find_all(&selected, "option:checked,[role='option']:checked"); // select all currently selected <option>s

    //if(selected.elements.size() == 0) 
    //std::vector<value_t> values(selected.elements.size());

    for( int n = selected.elements.size() - 1; n >= 0 ; --n )
      selected.elements[n].set_state(0, STATE_CHECKED); // reset values

    if(!val_array.is_array())
      return;

    for( int k = val_array.size(); k >= 0 ; --k )
    {
      std::wstring ws = val_array.nth(k).to_string();
      dom::element opt = el.find_first("option[value='%S'],[role='option'][value='%S']", ws.c_str()); // find it
      if( opt.is_valid() )
        opt.set_state( STATE_CHECKED | (k == 0? STATE_CURRENT:0)); // set state
    }
  }

  // clear checked states in multiselect <select>.
  // this simply resets :checked state for all checked <option>'s
  inline void clear_all_options(dom::element& select_el )
  {
    selected_cb selected;
    select_el.find_all(&selected, "option:checked,[role='option']:checked"); // select all currently selected <option>s

    for( int n = selected.elements.size() - 1; n >= 0 ; --n )
      selected.elements[n].set_state(0, STATE_CHECKED, false); // reset state

    select_el.update();
  }

  // selects all options in multiselect.
  inline void select_all_options(dom::element& select_el )
  {
    selected_cb all_options;
    select_el.find_all(&all_options, "option"); // select all currently selected <option>s

    for( int n = all_options.elements.size() - 1; n >= 0 ; --n )
       all_options.elements[n].set_state(STATE_CHECKED,0, false); // set state
    select_el.update();
  }

/** Get value of the DOM element. Returns value for elements recognized by get_ctl_type() function. 
 * \param[in] el \b const dom::element&, The element.
 * \return \b value_t, value of the element.
 **/
  inline value_t get_value(dom::element& el )
  {
    switch(get_ctl_type(el))
    {
      case CTL_UNKNOWN:         break;
      case CTL_EDIT:
      case CTL_DECIMAL:
      case CTL_CURRENCY:
      case CTL_PASSWORD:
        {
          dom::editbox ed = el;
          return value_t(ed.text_value());
        }
      case CTL_NUMERIC: 
        {
          dom::editbox ed = el;
          return value_t( ed.int_value() );
        } 
      case CTL_BUTTON:          
        return value_t(el.get_attribute("value"));
      case CTL_CHECKBOX:        return get_checkbox_bits(el);
      case CTL_RADIO:           return get_radio_index(el);
      case CTL_SELECT_SINGLE:   return get_select_value(el);
      case CTL_SELECT_MULTIPLE: return get_select_values(el);
      case CTL_DD_SELECT:       return get_select_value(dom::element(el.child(2))); 
                                // dropdown has 3 children: 0-caption, 1-button, 2-select, per se
      case CTL_TEXTAREA:        return value_t(el.get_text());
      case CTL_HTMLAREA:        return value_t(el.get_html(false/*inner*/));
      case CTL_PROGRESS:        return value_t(_wtoi(el.get_attribute("value")));
      case CTL_SLIDER:          return value_t(_wtoi(el.get_attribute("value")));
    }
    return value_t();
  }

/** Set value of the DOM element. Sets value for elements recognized by get_ctl_type() function. 
 * \param[in] el \b const dom::element&, The element.
 * \param[in] v \b const value_t&, The value.
 **/
  inline void set_value(dom::element& el, const value_t& v )
  {
    switch(get_ctl_type(el))
    {
      case CTL_UNKNOWN:         break;
      case CTL_EDIT: 
      case CTL_DECIMAL:
      case CTL_CURRENCY:
      case CTL_PASSWORD:
      case CTL_TEXTAREA:
        { 
          dom::editbox ed = el;
          ed.text_value(v.get( L"" ));
        } break;
      case CTL_NUMERIC:         
        {
          dom::editbox ed = el;
          ed.int_value(v.get(0));
        } break;
      case CTL_BUTTON:          break; 
      case CTL_CHECKBOX:        set_checkbox_bits(el,v);    break;
      case CTL_RADIO:           set_radio_index(el,v);  break;
      case CTL_SELECT_SINGLE:   set_select_value(el, v); break;
      case CTL_SELECT_MULTIPLE: set_select_values(el, v); break;
      case CTL_DD_SELECT:       set_select_value(dom::element(el.child(2)),v); break;
      case CTL_HTMLAREA:            
        {
          utf8::ostream os; os << v.get( L"" );
          el.set_html( os.data(), os.length() );
          el.update();
        } break;
      case CTL_PROGRESS:        
        {
          std::wstring str = v.to_string();
          el.set_attribute("value",str.c_str());
          el.update();
        } break;
      case CTL_SLIDER:
        {
          std::wstring str = v.to_string();
          el.set_attribute("value",str.c_str());
          el.update();
        } break;
      default:
        assert(false);
    }
  }

/** Collection (map) of Name/Value pairs. **/
  typedef std::map<std::wstring, value_t> named_values;

  typedef std::map<std::wstring, value_t>::iterator named_values_iterator;

/** Get values of all "controls" contained inside the DOM element. 
 *  Function will gather values of elements having name attribute defined
 *  and recognized by get_ctl_type() function.
 * \param[in] el \b dom::element&, The element.
 * \param[out] all \b named_values&, Collection.
 * \return \b bool, \c true if there are any value was harvested.
 **/
  inline bool get_values(const dom::element& el, named_values& all )
  {
    selected_cb selected;
    el.find_all(&selected, "[name]" ); // select all elements having name attribute
    for( unsigned int n = 0; n < selected.elements.size(); ++n )
    {
      const dom::element& t = selected.elements[n];
      if( !t.get_style_attribute("behavior") )
        continue;
      const wchar_t* pn = t.get_attribute("name");
      if( !pn )
      {
        assert(false); // how come?
        continue;
      }
      std::wstring name = pn;
      if( all.find(name) != all.end()) 
        continue; // element with this name is already there, 
                  // checkboxes and radios are groups in fact,
                  // we are returning here only cumulative group value
      if(get_ctl_type(t) == CTL_BUTTON)
        continue;
      all[name] = get_value(selected.elements[n]);
    }
    return all.size() != 0;
  }

/** Set values of all "controls" contained inside the DOM element by items 
 *  contained in the all colection. 
 *  Function will set values of elements having name attribute defined
 *  and recognized by get_ctl_type() function.
 * \param[in] el \b dom::element&, The element.
 * \param[out] all \b named_values&, Collection.
 * \return \b bool, \c true if there are any value was harvested.
 **/
  inline bool set_values(dom::element& el, named_values& all )
  {
    int counter = 0;
    for( named_values_iterator it = all.begin(); it != all.end(); it++ )
    {
      std::wstring name = (*it).first;
      //::MessageBoxW(NULL, name, L"" )
      dom::element t = el.find_first("[name='%S']",name.c_str()); 
      if( !t.get_style_attribute("behavior") )
        continue;
      set_value(t, (*it).second);
      ++counter;
    }
    return counter != 0;
  }

} //namespace htmlayout



#endif

