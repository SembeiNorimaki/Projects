$fn=100;
difference() {
union() {
    circle(r=27);
    translate([32,0]) circle(d=12);
    translate([26,0]) square([10,12],center=true);
    translate([-32,0]) circle(d=12);
    translate([-26,0]) square([10,12],center=true);
    translate([0,32]) circle(d=12);
    translate([0,26]) square([12,10],center=true);
    translate([0,-32]) circle(d=12);
    translate([0,-26]) square([12,10],center=true);
}
translate([33,0]) circle(r=1.7);
translate([-33,0]) circle(r=1.7);
translate([0,33]) circle(r=1.7);
translate([0,-33]) circle(r=1.7);
}
