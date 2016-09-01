
Drupal.behaviors.crossManageIntroDiagram = {
  attach: function (context, settings) {

    var width = 500,
      height = 500,
      margin = {left: 225, top:250 };

    var radius = {
      outerTrack: {
        outerRadius: 220,
        innerRadius: 200
      },
      middleTrack: {
        outerRadius: 140,
        innerRadius: 120
      },
      innerTrack: {
        outerRadius: 80,
        innerRadius: 60
      }
    };

    colors = {
        plan: '#004080',
        plant: '#80FF00',
        cross: '#449903',
        collect: '#245503',
        markers: '#547397'
    };

    // Create canvas.
    var svg = d3.select("#crossmanage-intro-stages-diagram").append("svg")
      .attr("width", width)
      .attr("height", height)
      .append('g')
        .attr('transform','translate(' + margin.left + ',' + margin.top + ')');

    // Function: draw arrows on end of arcs.
    var drawArrow = function(quadrant, track, innerRadius, outerRadius) {

      arrowLength = 40;
      flare = 10;
      arcWidth = outerRadius - innerRadius;

      if (track == 'outer') {
        arcRise = 3;
      } else if (track == 'middle') {
        arcRise = 7;
      } else if (track == 'inner') {
        arcRise = 14;
      }

      // Three points of the triangle.
      start = {};
      end = {};
      triangle = {one:{}, two:{},three:{}};
      if (quadrant == 2) {
        start.x = arrowLength;
        start.y = outerRadius - (arcWidth / 2) - arcRise;

        end.x = 0;
        end.y = outerRadius - (arcWidth / 2);

        triangle.one.x = start.x;
        triangle.one.y = start.y - (arcWidth / 2) - flare;
        triangle.two.x =  end.x;
        triangle.two.y = end.y;
        triangle.three.x = start.x;
        triangle.three.y = start.y + (arcWidth / 2) + flare;

      } else if (quadrant == 3) {
        start.x = -outerRadius + (arcWidth / 2) + arcRise;
        start.y = arrowLength;

        end.x = -outerRadius + (arcWidth / 2);
        end.y = 0;

        triangle.one.x = start.x + (arcWidth / 2) + flare;
        triangle.one.y = start.y;
        triangle.two.x =  end.x;
        triangle.two.y = end.y;
        triangle.three.x = start.x - (arcWidth / 2) - flare;
        triangle.three.y = start.y;

      } else if (quadrant == 4) {
        start.x = -arrowLength;
        start.y = -outerRadius + (arcWidth / 2) + arcRise;

        end.x = 0;
        end.y = -outerRadius + (arcWidth / 2);

        triangle.one.x = start.x;
        triangle.one.y = start.y - (arcWidth / 2) - flare;
        triangle.two.x =  end.x;
        triangle.two.y = end.y;
        triangle.three.x = start.x;
        triangle.three.y = start.y + (arcWidth / 2) + flare;
      }

      return 'M'+triangle.one.x+','+triangle.one.y
          +'L'+triangle.two.x+','+triangle.two.y
          +'L'+triangle.three.x+','+triangle.three.y
          +'L'+triangle.one.x+','+triangle.one.y;
    };

    // Outside Track: Planning
    //-------------------------------------------
    // Draw arc.
    planArc = d3.svg.arc()
        .innerRadius(radius.outerTrack.innerRadius)
        .outerRadius(radius.outerTrack.outerRadius)
        .startAngle(0 * (Math.PI/180))
        .endAngle(170 * (Math.PI/180));
    svg.append("path")
        .attr("d", planArc)
        .attr('fill',colors.plan);
    // Append arrow to end of arc.
    svg.append('path')
        .attr("d", drawArrow(2, 'outer', radius.outerTrack.innerRadius, radius.outerTrack.outerRadius))
        .attr('fill',colors.plan);
    // Draw the label "bloop" at the beginning of the arc.
    svg.append('path')
        .attr('d', 'M-15,-220 C30,-140 110,-110 130,-160 L100,-190 L0,-220')
        .attr('stroke', colors.plan)
        .attr('fill', colors.plan);
    // Label the "bloop".
    svg.append('text')
        .text('Planning')
        .attr('x', 60)
        .attr('y', -180)
        .attr('text-anchor', 'middle')
        .attr('fill', 'white');
    svg.append('text')
        .text('Crosses')
        .attr('x', 80)
        .attr('y', -160)
        .attr('text-anchor', 'middle')
        .attr('fill', 'white');

    // Middle Track: Planting Parents.
    //-------------------------------------------
    // Draw arc.
    plantArc = d3.svg.arc()
        .innerRadius(radius.middleTrack.innerRadius)
        .outerRadius(radius.middleTrack.outerRadius)
        .startAngle(90 * (Math.PI/180))
        .endAngle(170 * (Math.PI/180));
    svg.append("path")
        .attr("d", plantArc)
        .attr('fill', colors.plant);
    // Append arrow to end of arc.
    svg.append('path')
        .attr("d", drawArrow(2, 'middle', radius.middleTrack.innerRadius, radius.middleTrack.outerRadius))
        .attr('fill',colors.plant);
    // Draw the label "bloop" at the beginning of the arc.
    svg.append('path')
        .attr('d', 'M140,0 C215,10 195,80 110,85')
        .attr('stroke', colors.plant)
        .attr('fill', colors.plant);
    // Label the "bloop".
    svg.append('text')
        .text('Plant')
        .attr('x', 145)
        .attr('y', 30)
        .attr('text-anchor', 'middle')
        .attr('fill', colors.plan);
    svg.append('text')
        .text('Parents')
        .attr('x', 145)
        .attr('y', 50)
        .attr('text-anchor', 'middle')
        .attr('fill', colors.plan);

    // Middle Track: Making Crosses.
    //-------------------------------------------
    // Draw arc.
    crossArc = d3.svg.arc()
        .innerRadius(radius.middleTrack.innerRadius)
        .outerRadius(radius.middleTrack.outerRadius)
        .startAngle(180 * (Math.PI/180))
        .endAngle(260 * (Math.PI/180));
    svg.append("path")
        .attr("d", crossArc)
        .attr('fill', colors.cross);
    // Append arrow to end of arc.
    svg.append('path')
        .attr("d", drawArrow(3, 'middle', radius.middleTrack.innerRadius, radius.middleTrack.outerRadius))
        .attr('fill',colors.cross);
    // Draw the label "bloop" at the beginning of the arc.
    svg.append('path')
        .attr('d', 'M-90,105 C-85,200 0,200 0,140')
        .attr('stroke', colors.cross)
        .attr('fill', colors.cross);
    // Label the "bloop".
    svg.append('text')
        .text('Make')
        .attr('x', -48)
        .attr('y', 140)
        .attr('text-anchor', 'middle')
        .attr('fill', 'white');
    svg.append('text')
        .text('Crosses')
        .attr('x', -38)
        .attr('y', 160)
        .attr('text-anchor', 'middle')
        .attr('fill', 'white');

    // Middle Track: Collecting Seed.
    //-------------------------------------------
    // Draw arc.
    collectArc = d3.svg.arc()
        .innerRadius(radius.middleTrack.innerRadius)
        .outerRadius(radius.middleTrack.outerRadius)
        .startAngle(270 * (Math.PI/180))
        .endAngle(350 * (Math.PI/180));
    svg.append("path")
        .attr("d", collectArc)
        .attr('fill', colors.collect);
    // Append arrow to end of arc.
    svg.append('path')
        .attr("d", drawArrow(4, 'middle', radius.middleTrack.innerRadius, radius.middleTrack.outerRadius))
        .attr('fill',colors.collect);
    // Draw the label "bloop" at the beginning of the arc.
    svg.append('path')
        .attr('d', 'M-140,0 C-215,-10 -195,-80 -110,-85')
        .attr('stroke', colors.collect)
        .attr('fill', colors.collect);
    svg.append('text')
    // Label the "bloop".
        .text('Collect')
        .attr('x', -145)
        .attr('y', -45)
        .attr('text-anchor', 'middle')
        .attr('fill', 'white');
    svg.append('text')
        .text('Seed')
        .attr('x', -150)
        .attr('y', -25)
        .attr('text-anchor', 'middle')
        .attr('fill', 'white');

    // Inner Track: Screening for Markers.
    //-------------------------------------------
    // Draw arc.
    markerArc = d3.svg.arc()
        .innerRadius(radius.innerTrack.innerRadius)
        .outerRadius(radius.innerTrack.outerRadius)
        .startAngle(-60 * (Math.PI/180))
        .endAngle(150 * (Math.PI/180));
    svg.append("path")
        .attr("d", markerArc)
        .attr('fill', colors.markers);
    // Append arrow to end of arc.
    svg.append('path')
        .attr("d", drawArrow(2, 'inner', radius.innerTrack.innerRadius, radius.innerTrack.outerRadius))
        .attr('fill',colors.markers);
    // Draw the label "bloop" at the beginning of the arc.
    svg.append('path')
        .attr('d', 'M-52,-30 C-30,-10 30,0 40,-50 L-20,-70 L-50,-50')
        .attr('stroke', colors.markers)
        .attr('fill', colors.markers);
    // Label the "bloop".
    svg.append('text')
        .text('Screen')
        .attr('x', -10)
        .attr('y', -55)
        .attr('text-anchor', 'middle')
        .attr('fill', 'white');
    svg.append('text')
        .text('for Markers')
        .attr('x', -10)
        .attr('y', -35)
        .attr('text-anchor', 'middle')
        .attr('fill', 'white');
  }
};
