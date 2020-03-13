/////////////////////////
// USER CONTROL VARIABLES
/////////////////////////

// DEBUG FLAG - OUTPUT SERIAL INFORMATION TO MONITOR
const bool debug = false;

// INPUT PINS - ADD INPUT PINS TO THE ARRAY TO ADD SENSORS
const int pin_in[] = {3,5,6};

// OUTPUT PIN - SINGLE OUTPUT PIN IF RUNNING IN SERIAL - REWRITE FOR OTHER CONFIGS
const int pin_out[] = {12};

// MIN AND MAX DISTANCE THRESHOLDS - USED TO CALCULATE DEPTH FIELDS
// TODO - GENERALIZE FOR GREATER FIDELITY?
const int dist_min = 7;
const int dist_max = 12;

// SAMPLE BUFFER SIZE - NORMALIZES DISTANCE BEHAVIOR
const int buffer_length = 6;

// MIDI CHANNEL - CHANGE TO MATCH INPUT MIDI CHANNEL OF CONNECTED INTERPRETER
// DEFAULT IS 1
const int midi_channel = 1;

// DEFAULT NOTE OUTPUT FOR A NON-TRIGGERED STATE (FAR)
// NEAR- AND MID- FIELD NOTES GENERATED SEQUENTIALLY ABOVE THIS NOTE
// (MID) MIDI_NOTE_BASE + SENSOR + PIN_IN_LENGTH
// (NEAR) MIDI_NOTE_BASE + SENSOR
const int midi_note_base = 0x1E;

// EST. UPDATE DELAY NECESSARY FOR SERIALIZED INPUT
// TESTING INDICATES 72 AS MIMIMUM UPDATE FREQ TO GENERATE RELIABLE INPUT WITH 1 - 3 SENSORS
// DEFAULT 80 MS DELAY
const int wait_max = 80;

// EST. UPDATE MS DELAY PER SENSOR 
const int wait_sensor = 10;

/////////////////////////
// SYSTEM VARIABLES 
/////////////////////////

const int pin_in_length = (sizeof(pin_in)/sizeof(int));
const int pin_out_length = (sizeof(pin_out)/sizeof(int));
int buffer_matrix[pin_in_length][buffer_length], wait, dist, dist_calc, note, sensor;

/////////////////////////
// METHODS
/////////////////////////

// INITIAL SYSTEM CALL
void setup() {

	// TRADITIONAL SERIAL BAUD VS. MIDI STANDARD  
	// Serial.begin(9600);
	Serial.begin(31250);
  
	// INITIALIZE INPUT PINS
	for (int i = 0; i < pin_in_length; i++) {
    
		pinMode(pin_in[i], INPUT);

		// INITIALIZE INPUT BUFFERS
		for (int j = 0; j < buffer_length; j++) {
			buffer_matrix[i][j] = dist_max;  
		}
	}

	// INITIALIZE OUTPUT PINS
	for (int i = 0; i < pin_out_length; i++) {
		pinMode(pin_out[i], OUTPUT);
	}

	// INITIALIZE LOOP WAIT TIME
	// NOTE: CALC MAY FAIL AT SCALE, NOT TESTED WITH > 3 SENSORS
	if ((wait_max - (pin_in_length * wait_sensor)) > 0) {
		wait = wait_max - (pin_in_length * wait_sensor);
	} else {
		wait = 0;
	}
}

// PULSE OUT TO SENSORS
void write_sensor() {
	for (int i = 0; i < pin_out_length; i++) {
		digitalWrite(pin_out[i], HIGH);
		delay(1);
		digitalWrite(pin_out[i], LOW);  
	}
}

// INTERPRET SENSOR FEEDBACK
void read_sensor() {

	// RESET DEFAULTS
	dist = dist_max;
	sensor = 0;

	for (int i = 0; i < pin_in_length; i++) {

		// RESET DISTANCE CALCULATION PER SENSOR
		dist_calc = 0;
    
		// MARCH BUFFER
		for (int j = buffer_length - 1; j > 0; j--) {
			buffer_matrix[i][j] = buffer_matrix[i][j-1]; 
			dist_calc += buffer_matrix[i][j];
		}
    
		// STORE NEW SENSOR INPUT
		buffer_matrix[i][0] = ((pulseIn(pin_in[i], HIGH))/148)/12;

		// AVERAGE NEW SENSOR INPUT WITH EXISTING BUFFER VALUES
		dist_calc += buffer_matrix[i][0];
		dist_calc = floor(dist_calc/buffer_length);

		// SET LOWEST AVERAGE DISTANCE AND ASSOCIATED SENSOR
		if (dist_calc < dist) {
			dist = dist_calc;
			sensor = i;
		}

		// CLIENT - REPORTEDLY MADE DISTANCE READINGS MORE STABLE
		delay(wait_sensor);
	}

	// SET MIDI OUTPUT NOTE BASED ON SENSOR AND LOWEST DISTANCE
	if (dist >= dist_max) {
		note = midi_note_base; 
	} else if (dist < dist_min) { 
		note = midi_note_base + sensor + pin_in_length;  
	} else {
		note = midi_note_base + sensor;
	}
}

// DEBUG METHOD - REQUIRES CONNECTED ARDUINO IDE/TEENSY SERIAL OUTPUT
void debug_serial() {
	Serial.print(sensor);
	Serial.print("-");
	Serial.print(dist);
	Serial.println();  
}

// LOOPING SYSTEM CALL
void loop() {

	// PULSE OUT
	write_sensor();

	// INTERPRET FEEDBACK
	read_sensor();

	// UNCOMMENT FOR MIDI OUTPUT - REQUIRES TEENSY MIDI OUTPUT
	usbMIDI.sendNoteOn(note, 0x45, midi_channel);
  
	// UNCOMMENT FOR SERIAL DEBUG INFO
	// LEAVING UNCOMMENTED - SEEMS TO WORK IN MIDI OUTPUT FIRMWARE AS WELL
	if (debug) {
		debug_serial();
	}
  
	// WAIT CALCULATED IN SETUP
	delay(wait);
}

