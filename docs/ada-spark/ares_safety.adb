package body Ares_Safety
  with SPARK_Mode
is
   function Is_Valid_Reading
     (Value    : Float;
      Minimum  : Float;
      Maximum  : Float) return Boolean
   is
   begin
      return Value >= Minimum and Value <= Maximum;
   end Is_Valid_Reading;

   function Select_State
     (Thermal_Valid  : Boolean;
      Acoustic_Valid : Boolean;
      Seismic_Valid  : Boolean;
      Thermal_Hit    : Boolean;
      Acoustic_Hit   : Boolean;
      Seismic_Hit    : Boolean) return Mission_State
   is
   begin
      if not Thermal_Valid or else
        (not Acoustic_Valid and not Seismic_Valid)
      then
         return Degraded;
      elsif Thermal_Hit and then
        ((Acoustic_Valid and then Acoustic_Hit) or else
         (Seismic_Valid and then Seismic_Hit))
      then
         return Possible_Target;
      else
         return Searching;
      end if;
   end Select_State;

end Ares_Safety;
