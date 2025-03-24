JWPhaser : UGen {
	*ar { |input, rate, freq, q, mix, nonlin, gain, feedback, numStages|
		^this.multiNew('audio', input, rate, freq, q, mix, nonlin, gain, feedback, numStages);
	}
	checkInputs {
		^this.checkValidInputs;
	}
}
