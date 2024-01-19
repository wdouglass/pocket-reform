mirror([0,0,0]) union() {
    difference() {
        rotate([-90,0,0]) import("pref-assembly-20240112-BattHolder1-reexport.stl");

        // countersinks
        translate([-35.5, -8.5, -2.3])
        cylinder(h = 1.2, r = 3.8/2, $fn = 20);
        translate([17.7, -8.5, -2.3])
        cylinder(h = 1.2, r = 3.8/2, $fn = 20);
        
        // cut off a bit
        translate([-39, -13, -4]) cube([60,2,4]);
    }

    // left and right extensions
    translate([19.7, -11, -3.5])
    cube([0.5,7.5,2]);
}