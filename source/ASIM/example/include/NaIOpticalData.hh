// REFERENCES:
// IEEE Tr. on Nucl. Science. 44 (1997) 1237. (For optical absorption length)
// St Gobain: http://www.detectors.saint-gobain.com/NaI(Tl).aspx

// Refractive index
const G4int NaI_rIndexEntries = 2;
G4double NaI_rIndexSpectrum[2] = {1.5*eV, 5.*eV};
G4double NaI_rIndex[2] = {1.85, 1.85};

// Absorption length (see note at the bottom of this file)
const G4int NaI_absLengthEntries = 2;
G4double NaI_absLengthSpectrum[2] = {1.5*eV, 5*eV};
G4double NaI_absLength[2] = {30.*cm, 30.*cm};

// Fast rise time constant
const G4double NaI_fastRiseTimeC = 280*ns;

// Fast time decay contant
const G4double NaI_fastDecayTimeC = 250*ns;

// Optical photon yield per MeV deposited: 38000/MeV
const G4double NaI_yield = 3800/MeV;

// This parameter gives the relative strength of the "fast component"
// (of alpha/beta ratio) as a fraction of total scintillation yield
const G4double NaI_yRatio = 1.0;

// The number of optical photons scintillated is distributed in a
// gaussian around the scintillation yield.  This factor describes the
// "broadening" beyond the gaussian (1.0 = gaussian, 0 = no fluctuation). 
const G4double NaI_rScale = 1.0;

// Mean ionization energy for NaI. From examples/extended/TestEm15
const G4double NaI_meanIonisE = 452*eV;

// The NaI scintillation emission spectrum was taken directly from
// St. Gobain "Sodium Iodide Product Data Sheet" using a plot
// digitizer.  Please see ref/scintillationSpectrum for data sheet.

// Number of entries in scintillation emission spectrum arrays
const G4int NaI_entries = 35;

// Scintillation emission energy spectrum [eV].  Array is filled from
// NaI_eWavelength array in materialsLibrary::SodiumIodide
G4double NaI_eSpectrum[NaI_entries];

// Scintillation emission wavelength spectrum [nm]
G4double NaI_eWavelength[NaI_entries] = {318.16312*nm,
					 333.41452*nm,
					 345.04150*nm,
					 347.93597*nm,
					 368.29416*nm,
					 384.93771*nm,
					 387.80035*nm,
					 401.56534*nm,
					 402.21964*nm,
					 416.63492*nm,
					 420.22348*nm,
					 423.81204*nm,
					 431.07809*nm,
					 435.44568*nm,
					 441.27841*nm,
					 446.41443*nm,
					 447.91675*nm,
					 453.05008*nm,
					 454.53381*nm,
					 464.10242*nm,
					 471.49188*nm,
					 478.84421*nm,
					 480.34387*nm,
					 489.15866*nm,
					 494.31058*nm,
					 499.42801*nm,
					 501.64297*nm,
					 510.42593*nm,
					 518.46967*nm,
					 531.60034*nm,
					 555.65057*nm,
					 593.53223*nm,
					 640.17822*nm,
					 682.44336*nm,
					 720.33429*nm};


// Corresponding relative probability of emission as a function of energy
G4double NaI_eProb[NaI_entries] = {1.24640,
				   7.71674,
				   11.7398,
				   14.3614,
				   19.9605,
				   34.9915,
				   41.8058,
				   52.1188,
				   61.9019,
				   82.5221,
				   89.6861,
				   96.8501,
				   99.4737,
				   99.9997,
				   99.4781,
				   94.7637,
				   88.8247,
				   84.4596,
				   80.9664,
				   68.2179,
				   54.5950,
				   45.8635,
				   40.2740,
				   30.8444,
				   24.0335,
				   21.7648,
				   17.9225,
				   12.6855,
				   8.84572,
				   6.75513,
				   5.89216,
				   6.60748,
				   4.53153,
				   3.67652,
				   3.16898};

// I have had a great difficulty in determing the optical (of self)
// absorption length of NaI crystals despite much literature review.
// The paper referenced at the top of this file was the only concrete
// number given, so the present value will be set to the quoted number
// at 30 cm.  This number has a large role to play in determining the
// PHS of the cylindrical NaI-APD detector, since the light collection
// uniformity is highly influenced by the mean path length of induced
// scintillation photons.  
				   
