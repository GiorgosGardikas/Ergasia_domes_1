#include "set_utils.h"
#include <stdlib.h>

//Επιστρεφει την μοναδικη τιμη του set πο ειναι ισοδυναμη με value,
// η αν δεν υπαρχει, την μικροτερη τιμη του  set που ειναι μεγαλυτερη
// απο value. Αν δεν υπαρχει καμια τοτε επιστρεφει NULL.

Pointer set_find_eq_or_greater(Set set, Pointer value){

    if(set_find(set, value) != NULL){
        return value;
    }else{
        set_insert(set,value);
        SetNode node = set_find_node(set, value);

        if(set_next(set,node)!= SET_EOF ){
            return set_node_value(set, set_next(set,node));
        }else{
            return NULL;
        }
    }--    
}

/* Επιστρεφει την μοναδικη τιμη που ειναι ισοδυναμη με value η αν δεν υπαρχει,
την μεγαλυτερη τιμη του set που ειναι μικροτερη απο value. Αν δεν υπαρχει καμια τοτε επιστρεφει NULL */

Pointer set_find_eq_or_smaller(Set set, Pointer value){
    if(set_find(set, value) != NULL){
        return value;
    }else{
        set_insert(set,value);
        SetNode node = set_find_node(set, value);
        if(set_previous(set,node) != SET_BOF){
            return set_node_value(set,set_previous(set,node));
        }else{
            return NULL;
        }
    }
}