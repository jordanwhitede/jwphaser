JWPhaser : UGen {
	*ar { |input=0, freq=1000, q=1, mix=0.5, nonlin=0, gain=1, feedback=0.5, numStages=8|
		^this.multiNew('audio', input, freq, q, mix, nonlin, gain, feedback, numStages);
	}
	checkInputs {
		^this.checkValidInputs;
	}
}
