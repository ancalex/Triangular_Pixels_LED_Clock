#ifndef DISPLAY_FUNCTIONS_H_
#define DISPLAY_FUNCTIONS_H_

CRGBPalette16 currentPaletteBg( CRGB::Black );
CRGBPalette16 targetPaletteBg( CRGB::Black );
CRGBPalette16 currentPaletteTime( CRGB::Black );
CRGBPalette16 targetPaletteTime( CRGB::Black );

//code from https://gist.github.com/kriegsman/99082f66a726bdff7776
const CRGB lightcolor(8,5,1);

void softtwinkles() {
	for( int i = 0; i < NUM_LEDS; i++) {
		if( !leds[i]) continue; // skip black pixels
		if( leds[i].r & 1) { // is red odd?
			leds[i] -= lightcolor; // darken if red is odd
		} else {
			leds[i] += lightcolor; // brighten if red is even
		}
	}
	// Randomly choose a pixel, and if it's black, 'bump' it up a little.
	// Since it will now have an EVEN red component, it will start getting
	// brighter over time.
	if( random8() < 40) {
		int j = random16(NUM_LEDS);
		if( !leds[j] ) leds[j] = lightcolor;
	}
}

void pride()
{
	static uint16_t sPseudotime = 0;
	static uint16_t sLastMillis = 0;
	static uint16_t sHue16 = 0;

	uint8_t sat8 = beatsin88( 87, 220, 250);
	uint8_t brightdepth = beatsin88( 341, 96, 224);
	uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
	uint8_t msmultiplier = beatsin88(147, 23, 60);

	uint16_t hue16 = sHue16;//gHue * 256;
	uint16_t hueinc16 = beatsin88(113, 1, 3000);

	uint16_t ms = millis();
	uint16_t deltams = ms - sLastMillis ;
	sLastMillis  = ms;
	sPseudotime += deltams * msmultiplier;
	sHue16 += deltams * beatsin88( 400, 5,9);
	uint16_t brightnesstheta16 = sPseudotime;

	for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
		hue16 += hueinc16;
		uint8_t hue8 = hue16 / 256;

		brightnesstheta16  += brightnessthetainc16;
		uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

		uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
		uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
		bri8 += (255 - brightdepth);

		CRGB newcolor = CHSV( hue8, sat8, bri8);

		uint16_t pixelnumber = i;
		pixelnumber = (NUM_LEDS-1) - pixelnumber;

		nblend( leds[pixelnumber], newcolor, 64);
	}
}

void TimeInit() {
	for (int i = 0; i <= NUM_LEDS; i++) {
		led_list[i] = 0;
	}
	int minute_unit = DateTime.minute % 10;
	int minute_tenth = DateTime.minute / 10;
	int hour_unit = DateTime.hour % 10;
	int hour_tenth = DateTime.hour / 10;
	switch (minute_unit) {
	case 0: {
		led_list[2] = 1; led_list[3] = 1; led_list[4] = 1;
		led_list[13] = 1; led_list[16] = 1;
		led_list[25] = 1; led_list[26] = 1; led_list[27] = 1;
	}
	break;
	case 1: {
		led_list[3] = 1; led_list[4] = 1;
		led_list[13] = 1; led_list[14] = 1;
		led_list[27] = 1;
	}
	break;
	case 2: {
		led_list[2] = 1; led_list[3] = 1;
		led_list[13] = 1; led_list[14] = 1;led_list[15] = 1;led_list[16] = 1;
		led_list[26] = 1; led_list[27] = 1;
	}
	break;
	case 3: {
		led_list[2] = 1; led_list[3] = 1; led_list[4] = 1;
		led_list[13] = 1; led_list[14] = 1; led_list[15] = 1;
		led_list[26] = 1; led_list[27] = 1;
	}
	break;
	case 4: {
		led_list[3] = 1; led_list[4] = 1;
		led_list[13] = 1; led_list[14] = 1; led_list[15] = 1;
		led_list[25] = 1;
	}
	break;
	case 5: {
		led_list[2] = 1; led_list[3] = 1; led_list[4] = 1;
		led_list[14] = 1; led_list[15] = 1;
		led_list[25] = 1; led_list[26] = 1; led_list[27] = 1;
	}
	break;
	case 6: {
		led_list[2] = 1; led_list[3] = 1; led_list[4] = 1;
		led_list[14] = 1; led_list[15] = 1; led_list[16] = 1;
		led_list[25] = 1; led_list[26] = 1;
	}
	break;
	case 7: {
		led_list[3] = 1; led_list[4] = 1;
		led_list[13] = 1; led_list[14] = 1;
		led_list[25] = 1; led_list[26] = 1; led_list[27] = 1;
	}
	break;
	case 8: {
		led_list[2] = 1; led_list[3] = 1; led_list[4] = 1;
		led_list[13] = 1; led_list[14] = 1; led_list[15] = 1; led_list[16] = 1;
		led_list[25] = 1; led_list[26] = 1; led_list[27] = 1;
	}
	break;
	case 9: {
		led_list[3] = 1; led_list[4] = 1;
		led_list[13] = 1; led_list[14] = 1; led_list[15] = 1;
		led_list[25] = 1; led_list[26] = 1; led_list[27] = 1;
	}
	break;
	}
	switch (minute_tenth) {
	case 0: {
		led_list[17] = 1; led_list[18] = 1; led_list[19] = 1;
		led_list[21] = 1; led_list[24] = 1;
		led_list[42] = 1; led_list[43] = 1; led_list[44] = 1;
	}
	break;
	case 1: {
		led_list[19] = 1;
		led_list[21] = 1; led_list[22] = 1;
		led_list[43] = 1; led_list[44] = 1;
	}
	break;
	case 2: {
		led_list[18] = 1; led_list[19] = 1;
		led_list[21] = 1; led_list[22] = 1; led_list[23] = 1;led_list[24] = 1;
		led_list[42] = 1; led_list[43] = 1;
	}
	break;
	case 3: {
		led_list[17] = 1; led_list[18] = 1; led_list[19] = 1;
		led_list[22] = 1; led_list[23] = 1; led_list[24] = 1;
		led_list[42] = 1; led_list[43] = 1;
	}
	break;
	case 4: {
		led_list[17] = 1; led_list[18] = 1;
		led_list[22] = 1; led_list[23] = 1; led_list[24] = 1;
		led_list[44] = 1;
	}
	break;
	case 5: {
		led_list[17] = 1; led_list[18] = 1; led_list[19] = 1;
		led_list[22] = 1; led_list[23] = 1;
		led_list[42] = 1; led_list[43] = 1; led_list[44] = 1;
	}
	break;
	}
	//hour
	switch (hour_unit) {
	case 0: {
		led_list[57] = 1; led_list[58] = 1; led_list[59] = 1;
		led_list[64] = 1; led_list[67] = 1;
		led_list[84] = 1; led_list[85] = 1; led_list[86] = 1;
	}
	break;
	case 1: {
		led_list[58] = 1; led_list[59] = 1;
		led_list[64] = 1; led_list[65] = 1;
		led_list[86] = 1;
	}
	break;
	case 2: {
		led_list[57] = 1; led_list[58] = 1;
		led_list[64] = 1; led_list[65] = 1; led_list[66] = 1; led_list[67] = 1;
		led_list[85] = 1; led_list[86] = 1;
	}
	break;
	case 3: {
		led_list[57] = 1; led_list[58] = 1; led_list[59] = 1;
		led_list[64] = 1; led_list[65] = 1; led_list[66] = 1;
		led_list[85] = 1; led_list[86] = 1;
	}
	break;
	case 4: {
		led_list[58] = 1; led_list[59] = 1;
		led_list[64] = 1; led_list[65] = 1; led_list[66] = 1;
		led_list[84] = 1;
	break;
	}
	case 5: {
		led_list[57] = 1; led_list[58] = 1; led_list[59] = 1;
		led_list[65] = 1; led_list[66] = 1;
		led_list[84] = 1; led_list[85] = 1; led_list[86] = 1;
	}
	break;
	case 6: {
		led_list[57] = 1; led_list[58] = 1; led_list[59] = 1;
		led_list[65] = 1; led_list[66] = 1; led_list[67] = 1;
		led_list[84] = 1; led_list[85] = 1;
	}
	break;
	case 7: {
		led_list[58] = 1; led_list[59] = 1;
		led_list[64] = 1; led_list[65] = 1;
		led_list[84] = 1; led_list[85] = 1; led_list[86] = 1;
	}
	break;
	case 8: {
		led_list[57] = 1; led_list[58] = 1; led_list[59] = 1;
		led_list[64] = 1; led_list[65] = 1; led_list[66] = 1; led_list[67] = 1;
		led_list[84] = 1; led_list[85] = 1; led_list[86] = 1;
	}
	break;
	case 9: {
		led_list[58] = 1; led_list[59] = 1;
		led_list[64] = 1; led_list[65] = 1; led_list[66] = 1;
		led_list[84] = 1; led_list[85] = 1; led_list[86] = 1;
	}
	break;
	}
	switch (hour_tenth) {
	case 0: {
		led_list[68] = 1; led_list[69] = 1; led_list[70] = 1;
		led_list[80] = 1; led_list[83] = 1;
		led_list[89] = 1; led_list[90] = 1; led_list[91] = 1;
	}
	break;
	case 1: {
		led_list[70] = 1;
		led_list[80] = 1; led_list[81] = 1;
		led_list[90] = 1; led_list[91] = 1;
	}
	break;
	case 2: {
		led_list[69] = 1; led_list[70] = 1;
		led_list[80] = 1; led_list[81] = 1; led_list[82] = 1; led_list[83] = 1;
		led_list[89] = 1; led_list[90] = 1;
	}
	break;
	}
}

void TimeDisplay( uint8_t colorIndex)
{
	uint8_t brightness = 64;

	for( int i = 0; i < NUM_LEDS; i++) {
		if (led_list[i] == 1) {
		leds[i] = ColorFromPalette( currentPaletteTime, colorIndex + sin8(i*16), 200);
		colorIndex += 1;
		}
		else {
		leds[i] = ColorFromPalette( currentPaletteBg, colorIndex + sin8(i*16), brightness);
		colorIndex += 1;
	}
	}

}

#endif /* DISPLAY_FUNCTIONS_H_ */
