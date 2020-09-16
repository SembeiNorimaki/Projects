class Encoder {
private:	
	int pin;
	bool prev_value;
	long int prev_time;
	long int count = 0;
	const static int n = 10;
	unsigned long timing[n];
	int idx = 0;
public:
	Encoder(int);
	void read() {
		bool value = digitalRead(pin);
		if (value != prev_value) {
			timing[idx] = micros();
			if (idx < n-1)
				idx++;
			else
				idx = 0;
			prev_value = value;
			count++;
		}
	}
	long int get_count() {
		return count;
	}
	void print_timing() {
		for (int i=0; i<n; i++){
			Serial.print(timing[i]);
			Serial.print(" ");
		}
		Serial.println(" ");
		for (int i=1; i<n; i++){
			Serial.print(timing[i] - timing[i-1]);
			Serial.print(" ");
		}
		Serial.println(" ");
	}
};

Encoder::Encoder(int pin_) {
	pin = pin_;
}

Encoder e = Encoder(4);

void setup() {
	delay(1000);
	Serial.begin(115200);
	Serial.println(e.get_count());
}

void loop() {
	e.read();
	e.print_timing();
	delay(1000);
}

