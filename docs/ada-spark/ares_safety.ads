package Ares_Safety
  with SPARK_Mode
is
   type Mission_State is (Safe, Searching, Possible_Target, Degraded);

   function Is_Valid_Reading
     (Value    : Float;
      Minimum  : Float;
      Maximum  : Float) return Boolean
   with
     Pre  => Minimum <= Maximum,
     Post => Is_Valid_Reading'Result =
       (Value >= Minimum and Value <= Maximum);

   -- Classifies validated evidence only; it is not a detection guarantee.
   -- This package must not control flight actuators.
   function Select_State
     (Thermal_Valid  : Boolean;
      Acoustic_Valid : Boolean;
      Seismic_Valid  : Boolean;
      Thermal_Hit    : Boolean;
      Acoustic_Hit   : Boolean;
      Seismic_Hit    : Boolean) return Mission_State
   with
     Post =>
       (if not Thermal_Valid or
           (not Acoustic_Valid and not Seismic_Valid)
        then Select_State'Result = Degraded
        elsif Thermal_Hit and
          ((Acoustic_Valid and Acoustic_Hit) or
           (Seismic_Valid and Seismic_Hit))
        then Select_State'Result = Possible_Target
        else Select_State'Result = Searching);

end Ares_Safety;
