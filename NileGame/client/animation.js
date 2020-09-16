class Animation {
    constructor(ori, dst, speed) {
        this.pos = ori;
        this.step = {
            x: (dst.x - ori.x) / speed,
            y: (dst.y - ori.y) / speed,
        }

        this.nextPos = function() {
            this.pos.x += this.step.x;
            this.pos.y += this.step.y;
            return this.pos;
        }
    }
}