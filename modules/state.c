
#include <stdlib.h>

#include "ADTList.h"
#include "state.h"


// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

struct state {
	List objects;			// περιέχει στοιχεία Object (Εδαφος / Ελικόπτερα / Πλοία/ Γέφυρες)
	struct state_info info;	// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού
	float speed_factor;		// Πολλαπλασιαστής ταχύτητς (1 = κανονική ταχύτητα, 2 = διπλάσια, κλπ)
};


// Δημιουργεί και επιστρέφει ένα αντικείμενο

static Object create_object(ObjectType type, float x, float y, float width, float height) {
	Object obj = malloc(sizeof(*obj));
	obj->type = type;
	obj->rect.x = x;
	obj->rect.y = y;
	obj->rect.width = width;
	obj->rect.height = height;
	return obj;
}

void printObject(Object object){
	printf("Object data: \n ")
}



// Προσθέτει αντικείμενα στην πίστα (η οποία μπορεί να περιέχει ήδη αντικείμενα).
// Τα αντικείμενα ξεκινάνε από y = start_y, και επεκτείνονται προς τα πάνω.
//
// ΠΡΟΣΟΧΗ: όλα τα αντικείμενα έχουν συντεταγμένες x,y σε ένα
// καρτεσιανό επίπεδο.
// - Στο άξονα x το 0 είναι το αριστερό μέρος της πίστας και οι συντεταγμένες
//   μεγαλώνουν προς τα δεξιά. Η πίστα έχει σταθερό πλάτος, οπότε όλες οι
//   συντεταγμένες x είναι ανάμεσα στο 0 και το SCREEN_WIDTH.
//
// - Στον άξονα y το 0 είναι η αρχή της πίστας, και οι συντεταγμένες μεγαλώνουν
//   προς τα _κάτω_. Η πίστα αντιθέτως επεκτείνεται προς τα πάνω, οπότε όλες οι
//   συντεταγμένες των αντικειμένων είναι _αρνητικές_.
//
// Πέρα από τις συντεταγμένες, αποθηκεύουμε και τις διαστάσεις width,height
// κάθε αντικειμένου. Τα x,y,width,height ορίζουν ένα παραλληλόγραμμο, οπότε
// μπορούν να αποθηκευτούν όλα μαζί στο obj->rect τύπου Rectangle (ορίζεται
// στο include/raylib.h). Τα x,y αναφέρονται στην πάνω-αριστερά γωνία του Rectangle.

static void add_objects(State state, float start_y) {
	// Προσθέτουμε BRIDGE_NUM γέφυρες.
	// Στο διάστημα ανάμεσα σε δύο διαδοχικές γέφυρες προσθέτουμε:
	// - Εδαφος, αριστερά και δεξιά της οθόνης (με μεταβαλλόμενο πλάτος).
	// - 3 εχθρούς (ελικόπτερα και πλοία)
	// Τα αντικείμενα έχουν SPACING pixels απόσταση μεταξύ τους.

	for (int i = 0; i < BRIDGE_NUM; i++) {
		// Δημιουργία γέφυρας
		Object bridge = create_object(
			BRIDGE,
			0,								// x στο αριστερό άκρο της οθόνης
			start_y - 4 * (i+1) * SPACING,	// Η γέφυρα i έχει y = 4 * (i+1) * SPACING
			SCREEN_WIDTH,					// Πλάτος ολόκληρη η οθόνη
			20								// Υψος
		);

		// Δημιουργία εδάφους
		Object terrain_left = create_object(
			TERRAIN,
			0,								// Αριστερό έδαφος, x = 0
			bridge->rect.y,					// y ίδιο με την γέφυρα
			rand() % (SCREEN_WIDTH/3),		// Πλάτος επιλεγμένο τυχαία
			4*SPACING						// Υψος καλύπτει το χώρο ανάμεσα σε 2 γέφυρες
		);
		int width = rand() % (SCREEN_WIDTH/2);
		Object terrain_right = create_object(
			TERRAIN,
			SCREEN_WIDTH - width,			// Δεξί έδαφος, x = <οθόνη> - <πλάτος εδάφους>
			bridge->rect.y,					// y ίδιο με τη γέφυρα
			width,							// Πλάτος, επιλεγμένο τυχαία
			4*SPACING						// Υψος καλύπτει το χώρο ανάμεσα σε 2 γέφυρες
		);

		list_insert_next(state->objects, list_last(state->objects), terrain_left);
		list_insert_next(state->objects, list_last(state->objects), terrain_right);
		list_insert_next(state->objects, list_last(state->objects), bridge);

		// Προσθήκη 3 εχθρών πριν από τη γέφυρα.
		for (int j = 0; j < 3; j++) {
			// Ο πρώτος εχθρός βρίσκεται SPACING pixels κάτω από τη γέφυρα, ο δεύτερος 2*SPACING pixels κάτω από τη γέφυρα, κλπ.
			float y = bridge->rect.y + (j+1)*SPACING;

			Object enemy = rand() % 2 == 0		// Τυχαία επιλογή ανάμεσα σε πλοίο και ελικόπτερο
				? create_object(WARSHIP,    (SCREEN_WIDTH - 83)/2, y, 83, 30)		// οριζόντιο κεντράρισμα
				: create_object(HELICOPTER, (SCREEN_WIDTH - 66)/2, y, 66, 25);
			enemy->forward = rand() % 2 == 0;	// Τυχαία αρχική κατεύθυνση

			list_insert_next(state->objects, list_last(state->objects), enemy);
		}
	}
}

// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));

	// Γενικές πληροφορίες
	state->info.playing = true;				// Το παιχνίδι ξεκινάει αμέσως
	state->info.paused = false;				// Χωρίς να είναι paused.
	state->info.score = 0;					// Αρχικό σκορ 0
	state->info.missile = NULL;				// Αρχικά δεν υπάρχει πύραυλος
	state->speed_factor = 1;				// Κανονική ταχύτητα

	// Δημιουργία του αεροσκάφους, κεντραρισμένο οριζόντια και με y = 0
	state->info.jet = create_object(JET, (SCREEN_WIDTH - 35)/2,  0, 35, 40);

	// Δημιουργούμε τη λίστα των αντικειμένων, και προσθέτουμε αντικείμενα
	// ξεκινώντας από start_y = 0.
	state->objects = list_create(NULL);
	add_objects(state, 0);

	return state;
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state

StateInfo state_info(State state) {
	return state->info;
}

// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η συντεταγμένη y είναι ανάμεσα στο y_from και y_to.

List state_objects(State state, float y_from, float y_to) {
	
	List positive_y = list_create(NULL);
	ListNode 1_node = LIST_EOF;
	object obj = malloc(sizeof(*obj));

	//Διατρεχουε τπ list objects και για καθε αντικειμενο ελεγχουμε αν οι διαστασεις ειναι αναμεσα στo y_from και y_to
	for ( ListNode 1_node = list_first &(state->obejcts); 1_node != LIST_EOF; 1_node =  list_next(&state->object, 1_node;)){
		obj = list_node_value(state->object, 1_node);
		float begin_y = obj->rect.y;	
		float end_y = begin_y +obj->rect.height;
		if(y_from <= begin_y && end_y <= y_to){
			list_insert_next(pos_y, 1_node, list_node_value(state->objects,1_node));

		}
	}
	free(obj);
	return positive_y;
}
//Ελεγχει αν ο χαρακτηρας / ενας εχθρος συγκρουεται με κατι. Γυριζει το object με το οποιο συγκρουεται , η NULL αν δεν συγκρουστηκε με τιποτα
Object collision (State state, Object obj){
	Object obj_2 = malloc(sizeof(obj_2));

	for (ListNode node = list_first(state->objects); node != LIST_EOF; node = list_next(state->objects, node)){
		obj_2 = list_node_value(state->object,node);
		if(obj_2->type == 0 || obj_2->typem == 1){
			if(CheckCollisionRecs(obj->rect,obj_2->rect)){
				return obj_2;
			}
		}
	}
	return NULL;
} 
//Ενημερωνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.

void state_update(State state, KeyState keys) {
	// Αν πατηθει το p τοτε γινετσι pause\unpaused το παιχνιδι 
	if(IsKeyPressed(80)){
		state->info.paused = !state->info.paused;
}

//Οσο το παιχνιδι ειναι paused δεν εκτελειται η state_update
	if(state->info.paused && !keys->n){
	return;
}

object obj = malloc(sizeofof(*obj));
object obj = malloc(sizeof(*obj));
}
//Οταν δεν ειναι κανενα πληκτρο πατημενο η ειναι και τα δυο, ο χαρακτηρας παει 3 pixel προς την κατευθηνση στην οποια κινειται
if(!keys->up){
	state->info.jet->rect.y == state->info.jet->rect.x + 3;
}
// Πανω πληκτρο πατημενο τοτε το  τζετ κινειται με διπλη ταχυτητα
if(keys->up){
	state->info.jet->rect.y = state->info.jet->rect.y  * 2;
} 
// Οταν ειναι πατημενο το κατω πληκτρο , το τζετ κινεται με ταχυτητα 2 pixel ανα frame
if (keys->down){
	state->info.jet->rect.y = 2;
}
//Οταν ειναι πατημενο το δεξι η το αριστερο πληκτρο το τζετ κινειται με 3 pixel δεξια η αριστερα αντιστοιχα
if(keys->right || keys->left){
	if(state->info.jet->forward == false){
		state->info.jet->rect.y = state->info.jet->rect.y+2;
	} else{
		state->info.jet->rect.x = state->info.jet->rect.x + 3;
		state->info.jet->forward = false;
	}
} 
// Διατρεχουμε τη λιστα objects για να βρουμε τους εχθρους
 for(ListNode node = list_first(state->object); node = !LIST_EOF; node = list_next(state->objects,node)){
	 obj = list_node_value(dtate->objects,node);


	//Αν ο εχθρος ειναι πλοιο κινειται 3 pixel προς την κατευθηνση της κινησης του , ενω αν ειναι ελικοπτερο κινειται 4 pixel
	//προς την κατευθηνση της κινησης του, και τα δυο ειδη εχθρων αλλαζουν κατευθηνση οταν συγκρουονται με τις οχθες του ποταμου
	// ο εχθρος ειναι πλοιο
		if(obj->type == 3){
			if(!obj->forward){
				obj->rect.x = obj->rect.x - 3;
			}else{
				obj->rect.x = obj->rect.x + 3;
			}

			obj_2 = collision(state,obj);
		if(obj_2 != NULL){
			if(obj_2->type == 0){
				if(obj->forward){
					obj->forward = true;
				}else{
					obj-> forward = false;
				}
			}
		}
	
	}

	// ο εχθρος ειναι ελικοπτερο 
	if(obj -> type == 2){
		if(obj -> forward){
			obj -> rect.x = obj-> rect.x - 4;
		}else{
			obj -> rect.x = obj->rect.x + 4;
		}
		obj_2 = collision(state,obj);
		if(obj_2 != NULL){
			if(obj_2->typre == 0){
				if(!obj->forward){
					obj->forward = true;
				}else {
					obj->forward = false;
				}
			}
		}
	}
	//Συγκρουση του χαρακτηρα με οχθη/εχθρο(game over)

	obj = collision(state, state->info.jet);
	if(obj != NULL){
		if(obj->type == 0|| obj->type == 2|| obj->type ==5){
			state->info.playing = false;
		}
	}
	//Αν το παιχνιδι εχει τελειωσει,ξαναξεκινα πατωντας το enter
	if(!state->info.playing){
		if(keys->enter){
			state_create();
		}
	}
}
// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {
	free(state,state->info);
}