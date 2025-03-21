JWPhaser : UGen {
	*ar { |input, rate, depth, mix, feedback|
		^this.multiNew('audio', input, rate, depth, mix, feedback);
	}
	checkInputs {
		^this.checkValidInputs;
	}
}
