# Data representation

## Common structures

* `point = {"x": double, "y": double}` - point's structure;
* `color = {"r": double, "g": double, "b": double}` - color's structure (RGB).

## Bezier stroke

Parametres:
* `p0 | [x0, y0]` - start point;
* `p1 | [x1, y1]` - control point;
* `p2 | [x2, y2]` - end point;
* `width` - stroke's width;
* `color | color_id` - stroke's color or its id.

### Bezier stroke genome

```
stroke_genome = [
  x0: <double>,
  y0: <double>,
  x1: <double>,
  y1: <double>,
  x2: <double>,
  y2: <double>,
  width: <double>,
  color_id: <int>
]
```

Genome may not have `color_id` if colors are calculated during fitness function.

### Bezier stroke structure

```
stroke = {
  "type": "bezier",
  "p0": {
    "x": <double>,
    "y": <double>
  },
  "p1": {
    "x": <double>,
    "y": <double>
  },
  "p2": {
    "x": <double>,
    "y": <double>
  },
  "width": <double>
}
```

### Colored bezier stroke structure

```
colored_stroke = {
  "type": "bezier",
  "p0": {
    "x": <double>,
    "y": <double>
  },
  "p1": {
    "x": <double>,
    "y": <double>
  },
  "p2": {
    "x": <double>,
    "y": <double>
  },
  "width": <double>,
  "color": {"r": double, "g": double, "b": double}
}
```

If you have a pallete of colors, the `"color_id": int` replaces `"color"`.

## Stroke-list

```
{
  "colors": [
    {"r": <double>, "g": <double>, "b": <double>},
    {"r": <double>, "g": <double>, "b": <double>},
    ...
    {"r": <double>, "g": <double>, "b": <double>}
  ],
  "strokes": [
    <colored_stroke>
    <colored_stroke>
    ...
    <colored_stroke>
  ]
}
```

<details>

<summary>Example</summary>

```json
{
  "colors": [
    {"r": 240, "g": 80, "b": 100},
    {"r": 20, "g": 60, "b": 240},
    {"r": 30, "g": 150, "b": 40}
  ],
  "strokes": [
    {
      "type": "bezier",
      "p0": {
        "x": 100.20,
        "y": 40.25
      },
      "p1": {
        "x": 139.50,
        "y": 53
      },
      "p2": {
        "x": 134.30,
        "y": 82.80
      },
      "width": 20.40,
      "color_id": 2
    },
    {
      "type": "bezier",
      "p0": {
        "x": 88.70,
        "y": 50.20
      },
      "p1": {
        "x": 91.00,
        "y": 70.20
      },
      "p2": {
        "x": 122.6,
        "y": 79.90
      },
      "width": 16.80,
      "color_id": 0
    },
    {
      "type": "bezier",
      "p0": {
        "x": 94.30,
        "y": 48.50
      },
      "p1": {
        "x": 111.50,
        "y": 63.70
      },
      "p2": {
        "x": 115.7,
        "y": 82
      },
      "width": 18.00,
      "color_id": 1
    }
  ]
}
```

</details>