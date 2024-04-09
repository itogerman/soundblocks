void initMozzi() {
  startMozzi();

  frecuencyBase();
}

void frecuencyBase() {
  //select base frequencies using mtof (midi to freq) and fixed-point numbers
  f1 = Q16n16_mtof(Q16n0_to_Q16n16(58+modulation));
  f2 = Q16n16_mtof(Q16n0_to_Q16n16(84+modulation));
  f3 = Q16n16_mtof(Q16n0_to_Q16n16(74+modulation));
  f4 = Q16n16_mtof(Q16n0_to_Q16n16(87+modulation));
  f5 = Q16n16_mtof(Q16n0_to_Q16n16(77+modulation));
  f6 = Q16n16_mtof(Q16n0_to_Q16n16(67+modulation));
  // f7 = Q16n16_mtof(Q16n0_to_Q16n16(60));

  // set Oscils with chosen frequencies
  aCos1.setFreq_Q16n16(f1);
  aCos2.setFreq_Q16n16(f2);
  aCos3.setFreq_Q16n16(f3);
  aCos4.setFreq_Q16n16(f4);
  aCos5.setFreq_Q16n16(f5);
  aCos6.setFreq_Q16n16(f6);
  // aCos7.setFreq_Q16n16(f7);

  // set frequencies of duplicate oscillators
  aCos1b.setFreq_Q16n16(f1 + variation());
  aCos2b.setFreq_Q16n16(f2 + variation());
  aCos3b.setFreq_Q16n16(f3 + variation());
  aCos4b.setFreq_Q16n16(f4 + variation());
  aCos5b.setFreq_Q16n16(f5 + variation());
  aCos6b.setFreq_Q16n16(f6 + variation());
  //aCos7b.setFreq_Q16n16(f7+variation());
}
