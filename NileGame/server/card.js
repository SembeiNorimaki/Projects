function Card(name, idx, type) {
  this.name = name;
  this.idx = idx;
  this.type = type
  
  this.show = function(imgs) {
    image(imgs[this.idx], this.pos.x, this.pos.y, 100, 200);
  }
}