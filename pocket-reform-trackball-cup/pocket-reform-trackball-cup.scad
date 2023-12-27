inner_d = 18.2;
outer_d = 23;
bearing_d = 2.5;
res = 50;
num_bearings = 3;
delta = 360 / num_bearings;
bearing_circle_r = 7.8;
bearing_circle_y = -3;
tube_h = 4.5;
offset = 30;
screw_d = 1.85;
screw_x = 12.8;
screw_y = 9.05;
screw_y2 = 9.3;
plate_thickness = 3.1; // was 2.6

difference() {
    union() {
        difference() {
            union() {
                translate([0,0,2]) sphere(d = outer_d, $fn = res);
                translate([-6.12, 0, -tube_h - tube_h/2]) cylinder(h = tube_h, d=4, $fn = res, center = true);
                translate([ 6.12, 0, -tube_h - tube_h/2]) cylinder(h = tube_h, d=4, $fn = res, center = true);
            }
            translate([-4,-4,-11]) cube([8,8,9]);
            translate([-12,-12,0]) cube([24,24,14]);

            union() {
                for (i = [0:2]) {
                    phi = offset + i * delta;
                    translate([
                        bearing_circle_r * cos(phi),
                        bearing_circle_r * sin(phi),
                        bearing_circle_y])
                    sphere(d = 0.5+bearing_d, $fn=res);
                }
                translate([0,0,2]) sphere(d = inner_d, $fn=res);
                
                // tube screw cuts
                translate([-6.12, 0, -tube_h - tube_h/2]) cylinder(h = tube_h+2, d=screw_d, $fn = res, center = true);
                translate([ 6.12, 0, -tube_h - tube_h/2]) cylinder(h = tube_h+2, d=screw_d, $fn = res, center = true);
            }
        }

        // bearings
        /*for (i = [0:2]) {
            phi = offset + i * delta;
            translate([
                bearing_circle_r * cos(phi),
                bearing_circle_r * sin(phi),
                bearing_circle_y])
            sphere(d = bearing_d, $fn=res);
        }*/
        
        // plate
        translate([0,0.5,0]) difference() {
            translate([0, 0, -plate_thickness/2]) cube([28.5, 21.5, plate_thickness], center = true);
            translate([0,-0.5,0]) translate([0, 0, plate_thickness]) sphere(d = outer_d-0.5, $fn = res);
            
            translate([-screw_x, screw_y, 0]) cylinder(h = 8, d=screw_d, $fn = res, center = true);
            translate([ screw_x, screw_y, 0]) cylinder(h = 8, d=screw_d, $fn = res, center = true);
            translate([-screw_x,-screw_y2, 0]) cylinder(h = 8, d=screw_d, $fn = res, center = true);
            translate([ screw_x,-screw_y2, 0]) cylinder(h = 8, d=screw_d, $fn = res, center = true);
        }

        // trackball itself
        //translate([0,0,1]) sphere(d = 15, $fn=res);
    }
    
    // cut away outer sphere overhang (due to assymetry)
    translate([0,-21.5/2 - 0.5,-plate_thickness/2]) cube([30,2,plate_thickness+1], center = true);
    
    // cut view
    //translate([0,0,-10]) cube(20, center=false);
};

