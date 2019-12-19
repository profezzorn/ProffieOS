// Will load the 4 Armed General sound font from Kyberphonic
// Both High and Low versions
// High has fire blade
// Low has tron blade

{"GrievousH", "tracks/gull.wav",
    StylePtr<StyleFire<OrangeRed,Red,0,2,FireConfig<10,3000,4>,FireConfig<6000,100,0>,FireConfig
    <0,5000,10>,FireConfig<0,0,5>>>(), "Gen G \n  Fire"},

{"GrievousL", "tracks/rocks.wav",
    StylePtr<InOutHelper<ColorCycle<Chartreuse,0,1,Blast<SimpleClash<Lockup
    <Chartreuse,RandomPerLEDFlicker<Chartreuse,Yellow>,AudioFlicker<Chartreuse,GhostWhite
    >>,RandomPerLEDFlicker<Chartreuse,Yellow>,60>,RandomPerLEDFlicker<Chartreuse,GhostWhite>
    ,300,200,200>,100,3000,5000>,300,800>>(), "Gen G \n  Tron"},