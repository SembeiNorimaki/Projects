r1 = 45;
r2 = 12;
thick = 1.5;
module a() {

difference() {

union() {
rotate_extrude(convexity = 10, $fn=50)
translate([r1-(r2/2), 0, 0])
scale([1.0,2.0]) circle(r = r2/2, $fn = 50);


translate([0,r1-(r2/2)+15,0]) rotate([90,0,0]) cylinder(h=15, r1=8, r2=r2);
    
}
translate([0,r1-(r2/2)+15,0]) rotate([90,0,0]) cylinder(h=15, r1=8-thick, r2=r2-thick);
color([1,0,0]) rotate_extrude(convexity = 10, $fn=50)
translate([r1-(r2/2), 0, 0])
scale([1.0,2.0]) circle(r = r2/2-thick, $fn = 50);



translate([0,0,-50]) cube([100,100,100]);
translate([0,0,-50]) cylinder(h=2*r2+100,r = r1-(r2/2), $fn = 50);
}

difference() {
translate([0,0,-r2+thick]) cylinder(r=r1-(r2/2)+1,h=thick, $fn=100);
translate([0,0,-r2-10]) cylinder(r=r1-(r2/2)-2,h=thick+20);
}

difference() {
translate([0,0,r2-2*thick]) cylinder(r=r1-(r2/2)+1,h=thick, $fn=100);
translate([0,0,r2-10]) cylinder(r=r1-(r2/2)-2,h=thick+20);
}

// floor
color([0,0,1]) translate([0,0,-r2]) cylinder(h=thick, r=r1-(r2/2)+0.8);
// battery
translate([-14,0,-r2+2+thick]) cube([30,40,4],center=true);

//gps
translate([-14,0,-r2+thick+10]) cube([27,38,10],center=true);
//arduino
translate([14,0,-r2+thick+4]) cube([18,45,8],center=true);
//compass
translate([14,0,-r2+thick+12]) cube([10,10,6],center=true);


//color([0,1,0]) translate([0,0,r2-5]) cylinder(h=thick, r=r1-(r2/2)-3);



}


translate([-r1,0,0]) a();


