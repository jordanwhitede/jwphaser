JWPhaser : UGen {
	*ar { |input, rate, depth, mix, feedback, numStages|
		^this.multiNew('audio', input, rate, depth, mix, feedback, numStages);
	}
	checkInputs {
		^this.checkValidInputs;
	}
}
