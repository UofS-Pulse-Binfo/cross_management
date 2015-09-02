Drupal.behaviors.crossmanageRegisterCrossesProgress = {
  attach: function (context, settings) {

    var fontFamily = '"Helvetica Neue", Helvetica, Arial, sans-serif';

    // Define the chart dimensions & margins.
    var margins = {
        top: 50,
        left: 0,
        right: 0,
        bottom: 24
    };
    var contentWidth = jQuery('#content .section').width();
    var width = contentWidth - margins.right;
    var height = 35;

    // Define the dataset.
    // The progress bar consists of 2 portions, the correct and
    // incorrect items.
    var correct = Drupal.settings.crossManage.correct;
    var incorrect = Drupal.settings.crossManage.incorrect;
    // The total expected is the number of expected correct items
    // therefore we need to add the incorrect items to get the
    // total length of the bar when complete.
    var totalExpected = Drupal.settings.crossManage.expected;
    var total = totalExpected + incorrect;
debugger;
    // Finally, we need to determine the scale for the
    // progressbar based on the total count and the
    // size of our canvas.
    // Remember, domain is the min,max value of the input data
    // and range is the min,max number of pixels that data
    // should be scaled to.
    var xScale = d3.scale.linear()
      .domain([0, total])
      .range([0, width]);

    // Create a function to draw the 'Expected' line since it needs to be done often.
    var drawExpected = function() {
      svg.selectAll('.expected-line').remove();

      // If there is not enough room between the end of the graph and the expected line
      // to draw the label on the right side.
      condition1 = xScale(incorrect) < 50;
      // If there is not enough room between the length of the correct bar and the
      // expected line to draw the label on the left side.
      condition2 = (xScale(totalExpected) - xScale(correct)) < 50;

      line = svg.append('svg:line')
        .classed('expected-line', true)
        .attr('x1', xScale(totalExpected))
        .attr('y1', -13)
        .attr('x2', xScale(totalExpected))
        .attr('y2', height)
        .style('stroke', 'black');

      label = svg.append('text')
        .classed('expected-line', true)
        .attr('x', xScale(totalExpected)+2)
        .attr('y', -5)
        .text('Expected')
        .attr('font-family', fontFamily)
        .attr('font-size', '10px')
        .attr('text-anchor', 'start');

        // If condition1 (see top of function) then draw the label on the right side
        // of the expected line.
        if (condition1) {
          label.attr('x', xScale(totalExpected)-2)
            .attr('text-anchor','end');
        }

        // If condition1 & 2 (see top of function) then move the label above the
        // 'Requested' label while keeping it on the left side.
        if (condition1 && condition2) {
          label.attr('y', -20);
          line.attr('y1',-30);
        }
    };

    // Create the canvas.
    svg = d3.select('#proportion-based-stacked-progressbar')
      .append('svg')
      .attr('width', width + margins.left + margins.right)
      .attr('height', height + margins.top + margins.bottom)
      .append('g')
      .attr('transform', 'translate(' + margins.left + ',' + margins.top + ')');

    // Label our Chart!
    svg.append('text')
      .classed('correct', true)
      .attr('x', 0)
      .attr('y', -25)
      .text('Current Progress')
      .attr('font-family', fontFamily)
      .attr('font-weight', 'bold')
      .attr('font-style', 'italic')
      .attr('font-size', '16px')
      .attr('fill', 'black');

    // First we draw the empty bar.
    svg.append('rect')
      .classed('empty', true)
      .classed('progressbar', true)
      .attr('width', xScale(total))
      .attr('height', height)
      .attr('fill', '#F2F2F2');

    // Now indicate where we expect the correct bar to reach.
    drawExpected();

    // Now we finally get to draw our bars :).
    // First we draw the correct items bar.
    svg.append('rect')
      .classed('correct', true)
      .classed('progressbar', true)
      .attr('width', 0) // we start the rectangle out small so we can animate it :)
      .attr('height', height)
      .attr('fill', '#448E25')
    .transition()
      .attr('width', xScale(correct))
      .duration(1000)
      .delay(10);

    // Requested Label
    correctBar = svg.append('text')
      .classed('correct', true)
      .attr('x', 48)
      .attr('y', -5)
      .text('Requested')
      .attr('text-anchor','end')
      .attr('font-family', fontFamily)
      .attr('font-size', '10px')
      .attr('fill', '#448E25');
    if (xScale(correct)-5 > 48) {
      correctBar.transition()
        .attr('x', xScale(correct)-5)
        .duration(900)
        .delay(100);
    }

    // Correct Number of crosses Label.
    if (xScale(correct) > 40) {
      numLabel = svg.append('text')
        .classed('correct', true)
        .attr('x', 20)
        .attr('y', height/2)
        .text(correct)
        .attr('text-anchor','end')
        .attr('alignment-baseline', 'middle')
        .attr('font-family', fontFamily)
        .attr('font-weight','bold')
        .attr('font-size', '14px')
        .attr('fill', 'white');
      numLabel.transition()
        .attr('x', xScale(correct)-10)
        .duration(920)
        .delay(80);
    }

    // Redraw the expected line so we can see it.
    drawExpected();

    // Now we draw the incorrect items bar.
    // Notice that we set the x to the width of the correct items bar.
    if (incorrect) {
      svg.append('rect')
        .classed('incorrect', true)
        .classed('progressbar', true)
        .attr('x', xScale(correct))
        .attr('width', 0)
        .attr('height', height)
        .attr('fill', '#95979D')
      .transition()
        .attr('width', xScale(incorrect))
        .duration(1000)
        .delay(998);

      setTimeout(function(){
        svg.append('text')
          .classed('incorrect', true)
          .attr('x', 39)
          .attr('y', height+10)
          .text('Deviants')
          .attr('text-anchor','end')
          .attr('font-family', fontFamily)
          .attr('font-size', '10px')
          .attr('fill', '#95979D')
        .transition()
          .attr('x', xScale(correct+incorrect)-2)
          .duration(1000);

        // Redraw the expected line so we can see it.
        drawExpected();

        // Correct Number of crosses Label.
        if (xScale(incorrect) > 40) {
          numLabel = svg.append('text')
            .classed('incorrect', true)
            .attr('x', xScale(correct)+20)
            .attr('y', height/2)
            .text(incorrect)
            .attr('text-anchor','end')
            .attr('alignment-baseline', 'middle')
            .attr('font-family', fontFamily)
            .attr('font-weight','bold')
            .attr('font-size', '14px')
            .attr('fill', 'white');
          numLabel.transition()
            .attr('x', xScale(correct + incorrect)-10)
            .duration(920)
            .delay(80);
        }
      }, 998);
    }
}};